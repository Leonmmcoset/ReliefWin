#include "../platform/posix.h"
#include "file_io.h"
#include <libintl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <new>
namespace rt {
namespace {
bool fail(Text &error, const char *operation) {
    const int saved = errno;
    char message[384]; snprintf(message,sizeof(message),"%s：%s",gettext(operation),strerror(saved));
    errno = saved;
    error.set(message); return false;
}
FileStamp from_stat(const ro::Stat &s, uint64_t digest=0) {
    FileStamp t; t.exists=true; t.device=s.st_dev; t.inode=s.st_ino; t.size=s.st_size;
    t.mtime=s.st_mtim.tv_sec; t.mtime_ns=s.st_mtim.tv_nsec; t.ctime=s.st_ctim.tv_sec; t.ctime_ns=s.st_ctim.tv_nsec;
    t.digest=digest; return t;
}
bool same_stat(const ro::Stat &a, const ro::Stat &b) { return same_stamp(from_stat(a),from_stat(b)); }
bool parent(const char *path, int &dir, Text &name, Text &error) {
    if (!path || !*path || strlen(path)>4095) { error.set("文件路径为空或过长"); return false; }
    Text copy; if (!copy.set(path)) { error.set("内存不足"); return false; }
    char *p=strdup(copy.c_str());
    if (!p) { error.set("内存不足"); return false; }
    for (char *at=p; *at; ++at) if (*at=='\\') *at='/';
    size_t consumed=0; dir=ro::root_directory(p,consumed);
    bool ok=dir>=0;
    char *part=p+consumed; while (*part=='/') ++part;
    while (ok && *part) {
        char *end=strchr(part,'/');
        if (!end) { ok=strcmp(part,".") && strcmp(part,"..") && name.set(part); break; }
        *end=0;
        if (!strcmp(part,"..")) { error.set("文件路径不能包含上级目录"); ok=false; break; }
        if (*part && strcmp(part,".")) {
            int next=ro::openat(dir,part,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
            if (next<0) { fail(error,"不能打开父目录（不支持符号链接）"); ok=false; break; }
            ro::close(dir); dir=next;
        }
        part=end+1; while (*part=='/') ++part;
    }
    if (ok && name.empty()) { error.set("请选择一个文件名"); ok=false; }
    if (!ok) { if (error.empty()) fail(error,"无效的文件路径"); if (dir>=0) ro::close(dir); dir=-1; }
    free(p); return ok;
}
bool read_at(int,const char *,Bytes &,FileStamp &,Text &,bool);
bool export_target_allowed(const char *path,const char *document,Text &error) {
    if (!document || !*document) return true;
    int target_dir=-1, document_dir=-1; Text target_name, document_name;
    if (!parent(path,target_dir,target_name,error)) return false;
    if (!parent(document,document_dir,document_name,error)) { ro::close(target_dir); return false; }
    ro::Stat target_parent{}, document_parent{};
    bool ok=ro::fstat(target_dir,&target_parent)==0 && ro::fstat(document_dir,&document_parent)==0;
    if (!ok) fail(error,"不能核对导出目录");
    bool same=ok && target_parent.st_dev==document_parent.st_dev &&
        target_parent.st_ino==document_parent.st_ino && target_name==document_name;
    if (ok && !same) {
        FileStamp source, target; Bytes source_bytes, target_bytes;
        ok=read_at(document_dir,document_name.c_str(),source_bytes,source,error,true) &&
            read_at(target_dir,target_name.c_str(),target_bytes,target,error,true);
        // FAT/exFAT stat may synthesize an inode from the supplied path.
        // Comparing full content also catches case-folded aliases there;
        // conservatively protect an identical copy of the active document.
        same=ok && source.exists && target.exists &&
            ((source.device==target.device && source.inode==target.inode) ||
             (source_bytes.size()==target_bytes.size() &&
              (!source_bytes.size() || !memcmp(source_bytes.data(),target_bytes.data(),source_bytes.size()))));
    }
    ro::close(document_dir); ro::close(target_dir);
    if (same) { error.set("导出目标是当前文档，请选择其他文件"); return false; }
    return ok;
}
bool read_at(int dir, const char *name, Bytes &bytes, FileStamp &stamp, Text &error, bool missing_ok) {
    int fd=ro::openat(dir,name,O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
    if (fd<0) {
        if (missing_ok && errno==ENOENT) { stamp=FileStamp{}; bytes.clear(); return true; }
        return fail(error,"不能打开文件");
    }
    ro::Stat before{}, after{}, named{}; bool ok=ro::fstat(fd,&before)==0;
    if (!ok) fail(error,"不能读取文件信息");
    if (ok && (!S_ISREG(before.st_mode) || before.st_size<0 || uint64_t(before.st_size)>MaxFileBytes)) {
        error.set("只支持不超过 16 MiB 的普通文件"); ok=false;
    }
    Bytes buffer;
    if (ok && !buffer.resize(size_t(before.st_size))) { error.set("内存不足"); ok=false; }
    size_t at=0;
    while (ok && at<buffer.size()) {
        size_t count=buffer.size()-at; if (count>65536) count=65536;
        ssize_t n=ro::read(fd,buffer.data()+at,count);
        if (n<0 && errno==EINTR) continue;
        if (n<=0) { if (!n) error.set("文件已截断"); else fail(error,"读取失败"); ok=false; break; }
        at+=size_t(n);
    }
    if (ok && (ro::fstat(fd,&after)<0 || ro::fstatat(dir,name,&named,AT_SYMLINK_NOFOLLOW)<0 ||
        !S_ISREG(named.st_mode) || !same_stat(before,after) || !same_stat(after,named))) {
        error.set("文件在读取过程中被其他程序修改，请重试"); ok=false;
    }
    ro::close(fd);
    if (!ok) return false;
    uint64_t digest=UINT64_C(14695981039346656037);
    for (size_t n=0;n<buffer.size();++n) { digest^=static_cast<unsigned char>(buffer[n]); digest*=UINT64_C(1099511628211); }
    stamp=from_stat(after,digest); bytes.swap(buffer); return true;
}
bool write_all(int fd,const char *bytes,size_t size) {
    size_t at=0;
    while (at<size) {
        size_t count=size-at; if (count>65536) count=65536;
        ssize_t n=ro::write(fd,bytes+at,count);
        if (n<0 && errno==EINTR) continue;
        if (n<=0) { if (!n) errno=EIO; return false; }
        at+=size_t(n);
    }
    return true;
}
}
bool same_stamp(const FileStamp &a,const FileStamp &b) {
    return a.exists==b.exists && (!a.exists || (a.device==b.device && a.inode==b.inode && a.size==b.size &&
        a.digest==b.digest && a.mtime==b.mtime && a.mtime_ns==b.mtime_ns && a.ctime==b.ctime && a.ctime_ns==b.ctime_ns));
}
bool read_file(const char *path,Bytes &bytes,FileStamp &stamp,Text &error) {
    int dir=-1; Text name; error.clear(); if (!parent(path,dir,name,error)) return false;
    bool ok=read_at(dir,name.c_str(),bytes,stamp,error,false); ro::close(dir); return ok;
}
bool file_stamp(const char *path,FileStamp &stamp,Text &error) {
    int dir=-1; Text name; Bytes bytes; error.clear(); if (!parent(path,dir,name,error)) return false;
    bool ok=read_at(dir,name.c_str(),bytes,stamp,error,true); ro::close(dir); return ok;
}
bool atomic_save(const char *path,const Bytes &bytes,const FileStamp &expected,FileStamp &out,Text &error) {
    if (bytes.size()>MaxFileBytes) { error.set("文件超出 16 MiB 限制"); return false; }
    ro::SaveLock transaction;
    if (!transaction.acquire(path)) return fail(error,"不能取得文档保存锁");
    int dir=-1; Text name; error.clear(); if (!parent(path,dir,name,error)) return false;
    Bytes unused; FileStamp actual;
    bool ok=read_at(dir,name.c_str(),unused,actual,error,true);
    if (ok && !same_stamp(actual,expected)) { error.set("文件已被其他程序修改；请重新打开或另存为，避免覆盖"); ok=false; }
    char temp[64]{}; int fd=-1;
    for (unsigned n=0;ok && n<128;++n) {
        snprintf(temp,sizeof(temp),".relieftodo-%ld-%u.tmp",long(ro::getpid()),n);
        fd=ro::openat(dir,temp,O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,0600);
        if (fd>=0) break;
        if (errno!=EEXIST) { fail(error,"不能创建临时文件"); ok=false; }
    }
    if (ok && fd<0) { error.set("无法分配临时文件"); ok=false; }
    if (ok && ro::set_document_mode(fd,0600)<0) ok=fail(error,"不能设置文档权限");
    if (ok && (!write_all(fd,bytes.data(),bytes.size()) || ro::fsync(fd)<0)) ok=fail(error,"写入或同步失败");
    if (fd>=0 && ro::close(fd)<0 && ok) ok=fail(error,"关闭临时文件失败");
    bool renamed=false;
    if (ok) {
        ok=read_at(dir,name.c_str(),unused,actual,error,true);
        if (ok && !same_stamp(actual,expected)) { error.set("保存冲突：磁盘文件已更改，请另存为"); ok=false; }
    }
    if (ok) {
        if (ro::renameat(dir,temp,dir,name.c_str())<0) ok=fail(error,"替换文件失败");
        else renamed=true;
    }
    if (ok && ro::fsync(dir)<0) { error.set("文件已替换，但目录同步失败；请重新打开核对后再保存"); ok=false; }
    if (ok) ok=read_at(dir,name.c_str(),unused,out,error,false);
    if (fd>=0 && !renamed) ro::unlinkat(dir,temp,0);
    ro::close(dir); return ok;
}
struct IoJob::Work {
    Kind kind;
    Text path, protected_document;
    Bytes bytes;
    FileStamp expected;
    Reply reply;
};
void IoJob::perform(void *opaque) {
    Work &work=*static_cast<Work *>(opaque);
    Reply &reply=work.reply;
    bool ok=export_target_allowed(work.path.c_str(),work.protected_document.c_str(),reply.error);
    if (ok) ok=work.kind==Kind::Read?read_file(work.path.c_str(),reply.bytes,reply.stamp,reply.error):
        work.kind==Kind::Probe?file_stamp(work.path.c_str(),reply.stamp,reply.error):
        atomic_save(work.path.c_str(),work.bytes,work.expected,reply.stamp,reply.error);
    reply.ok=ok;
}
bool IoJob::start(Kind kind,const char *path,const Bytes *bytes,const FileStamp *expected,Text &error,const char *protected_document) {
    if (busy()) { error.set("文件操作正在进行"); return false; }
    Work *work=new (std::nothrow) Work;
    if (!work) { error.set("内存不足"); return false; }
    work->kind=kind;
    bool ok=path && work->path.set(path) && work->protected_document.set(protected_document?protected_document:"");
    if (ok && bytes) {
        ok=work->bytes.resize(bytes->size());
        if (ok && bytes->size()) memcpy(work->bytes.data(),bytes->data(),bytes->size());
    }
    if (!ok) { delete work; error.set("内存不足或文件路径无效"); return false; }
    if (expected) work->expected=*expected;
    if (!worker_.start(perform,work)) { delete work; return fail(error,"不能启动文件工作线程"); }
    work_=work; kind_=kind; finished_=false; reply_=Reply{}; error.clear(); return true;
}
bool IoJob::read(const char *path,Text &error) { return start(Kind::Read,path,nullptr,nullptr,error); }
bool IoJob::probe(const char *path,Text &error,const char *protected_document) { return start(Kind::Probe,path,nullptr,nullptr,error,protected_document); }
bool IoJob::write(const char *path,const Bytes &bytes,const FileStamp &expected,Text &error,const char *protected_document) { return start(Kind::Write,path,&bytes,&expected,error,protected_document); }
bool IoJob::poll() {
    if (!busy()) return false;
    if (finished_) return true;
    if (!worker_.finished()) return false;
    worker_.wait();
    reply_=static_cast<Reply &&>(work_->reply);
    delete work_; work_=nullptr; finished_=true;
    return true;
}
IoJob::Reply IoJob::take() {
    if (!finished_) return Reply{};
    Reply result=static_cast<Reply &&>(reply_); kind_=Kind::Idle; finished_=false; return result;
}
IoJob::~IoJob() {
    worker_.wait();
    delete work_;
}
}
