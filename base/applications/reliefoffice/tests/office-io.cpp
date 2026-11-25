/*
 * PROJECT:     ReliefOffice API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Exercise real native formats, Unicode IO, save conflicts and recovery
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include <platform/posix.h>
#include <platform/worker.h>
#include <reliefword/document.h>
#include <reliefexcel/document.h>
#include <reliefppt/document.h>
#include <relieftodo/file_io.h>
#include <reliefdiagram/document.h>
#include <reliefmathpp/document.h>
#include <atomic>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace {
unsigned checks=0, failures=0;
const char *fixtures=".", *directory=".";
bool public_volume=false;
void check(bool value,const char *name) {
    ++checks;
    if (!value) { ++failures; fprintf(stderr,"FAIL %s (errno=%d)\n",name,errno); }
}
bool path(char (&out)[4096],const char *base,const char *name) {
    int n=snprintf(out,sizeof(out),"%s/%s",base,name);
    return n>0 && size_t(n)<sizeof(out);
}
bool existing_directory(const char *file,const char *name) {
    ro::Stat st;
    if (ro::lstat(file,&st)<0) {
        fprintf(stderr,"Invalid %s directory \"%s\": %s\n",name,file,strerror(errno));
        return false;
    }
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr,"Invalid %s directory \"%s\": not a directory\n",name,file);
        return false;
    }
    return true;
}
bool read_bytes(const char *file,rt::Bytes &bytes) {
    int fd=ro::open(file,O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
    if (fd<0) return false;
    ro::Stat st;
    bool ok=ro::fstat(fd,&st)==0 && S_ISREG(st.st_mode) && st.st_size>=0 &&
        uint64_t(st.st_size)<=rt::MaxFileBytes && bytes.resize(size_t(st.st_size));
    size_t at=0;
    while (ok && at<bytes.size()) {
        ssize_t count=ro::read(fd,bytes.data()+at,bytes.size()-at);
        if (count<0 && errno==EINTR) continue;
        if (count<=0) { ok=false; break; }
        at+=size_t(count);
    }
    if (ro::close(fd)<0) ok=false;
    return ok;
}
bool write_bytes(const char *file,const char *data,size_t size) {
    int fd=ro::open(file,O_WRONLY|O_CREAT|O_TRUNC|O_CLOEXEC|O_NOFOLLOW,0600);
    if (fd<0) return false;
    bool ok=true;
    size_t at=0;
    while (at<size) {
        ssize_t count=ro::write(fd,data+at,size-at);
        if (count<0 && errno==EINTR) continue;
        if (count<=0) { ok=false; break; }
        at+=size_t(count);
    }
    if (ok && ro::fsync(fd)<0) ok=false;
    if (ro::close(fd)<0) ok=false;
    return ok;
}
bool hardlink(const char *target,const char *alias) {
#ifdef _WIN32
    wchar_t wide_target[4096],wide_alias[4096];
    if (target[0]=='/' && target[1] && target[2]==':') ++target;
    if (alias[0]=='/' && alias[1] && alias[2]==':') ++alias;
    if (!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,target,-1,wide_target,4096) ||
        !MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,alias,-1,wide_alias,4096)) return false;
    for (wchar_t *at=wide_target;*at;++at) if (*at==L'/') *at=L'\\';
    for (wchar_t *at=wide_alias;*at;++at) if (*at==L'/') *at=L'\\';
    return CreateHardLinkW(wide_alias,wide_target,nullptr)!=FALSE;
#else
    return ::link(target,alias)==0;
#endif
}
#ifdef _WIN32
struct ChildLock {
    PROCESS_INFORMATION process={};
    HANDLE ready=nullptr,release=nullptr;
    bool start(const char *file,bool abandon) {
        SECURITY_ATTRIBUTES attributes={sizeof(attributes),nullptr,TRUE};
        ready=CreateEventW(&attributes,TRUE,FALSE,nullptr);
        release=CreateEventW(&attributes,TRUE,FALSE,nullptr);
        wchar_t executable[4096],target[8192],command[16384];
        DWORD size=GetModuleFileNameW(nullptr,executable,4096);
        size_t bytes=strlen(file);
        if (!ready || !release || !size || size>=4096 || bytes>=4096) return false;
        static const wchar_t digits[]=L"0123456789abcdef";
        for (size_t i=0;i<bytes;++i) {
            unsigned char value=static_cast<unsigned char>(file[i]);
            target[2*i]=digits[value>>4]; target[2*i+1]=digits[value&15];
        }
        target[2*bytes]=0;
        int count=swprintf(command,16384,L"\"%ls\" %ls %ls %llu %llu",executable,
            abandon?L"--abandon-lock-child":L"--hold-lock-child",target,
            static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(ready)),
            static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(release)));
        if (count<=0 || count>=16384) return false;
        STARTUPINFOW startup={sizeof(startup)};
        if (!CreateProcessW(executable,command,nullptr,nullptr,TRUE,CREATE_NO_WINDOW,
            nullptr,nullptr,&startup,&process)) return false;
        HANDLE events[]={ready,process.hProcess};
        return WaitForMultipleObjects(2,events,FALSE,10000)==WAIT_OBJECT_0;
    }
    bool finish() {
        bool ok=true;
        if (process.hProcess) {
            if (release) SetEvent(release);
            if (WaitForSingleObject(process.hProcess,5000)!=WAIT_OBJECT_0) {
                TerminateProcess(process.hProcess,1);
                WaitForSingleObject(process.hProcess,5000);
                ok=false;
            }
            DWORD code=1;
            if (!GetExitCodeProcess(process.hProcess,&code) || code) ok=false;
            CloseHandle(process.hThread); CloseHandle(process.hProcess);
            process={};
        }
        if (ready) { CloseHandle(ready); ready=nullptr; }
        if (release) { CloseHandle(release); release=nullptr; }
        return ok;
    }
    ~ChildLock() { finish(); }
};
struct AbandonRequest { const char *file; bool ok=false; };
void abandon_lock(void *opaque) {
    AbandonRequest &request=*static_cast<AbandonRequest *>(opaque);
    ro::SaveLock *owner=new(std::nothrow) ro::SaveLock;
    if (owner && owner->acquire(request.file)) { request.ok=true; return; }
    delete owner;
}
int lock_child(const char *file_hex,const char *ready_text,const char *release_text,bool abandon) {
    size_t length=strlen(file_hex);
    char file[4096];
    if (!length || length%2 || length/2>=sizeof(file)) return 2;
    for (size_t i=0;i<length/2;++i) {
        unsigned value=0;
        for (size_t digit=0;digit<2;++digit) {
            char byte=file_hex[2*i+digit];
            if (byte>='0' && byte<='9') value=(value<<4)|unsigned(byte-'0');
            else if (byte>='a' && byte<='f') value=(value<<4)|unsigned(byte-'a'+10);
            else return 2;
        }
        if (!value) return 2;
        file[i]=char(value);
    }
    file[length/2]=0;
    char *end=nullptr;
    unsigned long long value=strtoull(ready_text,&end,10);
    if (!value || !end || *end || value>UINTPTR_MAX) return 2;
    HANDLE ready=reinterpret_cast<HANDLE>(static_cast<uintptr_t>(value));
    value=strtoull(release_text,&end,10);
    if (!value || !end || *end || value>UINTPTR_MAX) return 2;
    HANDLE release=reinterpret_cast<HANDLE>(static_cast<uintptr_t>(value));
    ro::SaveLock owner;
    if (abandon) {
        AbandonRequest request{file};
        ro::Worker worker;
        if (!worker.start(abandon_lock,&request)) return 3;
        worker.wait();
        if (!request.ok) return 3;
    } else if (!owner.acquire(file)) return 3;
    if (!SetEvent(ready)) return 4;
    return WaitForSingleObject(release,20000)==WAIT_OBJECT_0?0:4;
}
#endif
bool equal(const char *a,size_t an,const char *b,size_t bn) {
    return an==bn && (!an || !memcmp(a,b,an));
}
bool on_disk(const char *file,const char *bytes,size_t size) {
    rt::Bytes actual;
    return read_bytes(file,actual) && equal(actual.data(),actual.size(),bytes,size);
}
bool fixture(const char *name,rt::Bytes &bytes) {
    char file[4096];
    bool ok=path(file,fixtures,name) && read_bytes(file,bytes);
    check(ok,name);
    return ok;
}
template<class Decode>
void malformed(const rt::Bytes &bytes,Decode decode) {
    size_t limit=bytes.size()<96?bytes.size():96;
    for (size_t n=0;n<limit;++n) check(!decode(bytes.data(),n),"truncated native header/body rejected");
    if (bytes.size()>96) check(!decode(bytes.data(),bytes.size()/2),"truncated native midpoint rejected");
    if (bytes.size()) check(!decode(bytes.data(),bytes.size()-1),"truncated final byte rejected");
    rt::Bytes bad;
    if (!bad.resize(bytes.size()+1)) { check(false,"malformed buffer allocation"); return; }
    if (bytes.size()) memcpy(bad.data(),bytes.data(),bytes.size());
    bad[bytes.size()]=0;
    check(!decode(bad.data(),bad.size()),"trailing native data rejected");
    if (bytes.size()>=16) {
        bad[0]^=0x40;
        check(!decode(bad.data(),bytes.size()),"invalid native magic rejected");
        bad[0]^=0x40;
        bad[4]=char(0xff);
        check(!decode(bad.data(),bytes.size()),"unsupported native version rejected");
        memcpy(bad.data(),bytes.data(),bytes.size());
        memset(bad.data()+8,0xff,8);
        check(!decode(bad.data(),bytes.size()),"invalid native length/checksum rejected");
    }
}
struct Cancel { unsigned calls=0; };
bool cancel_after_progress(void *opaque,unsigned done,unsigned) {
    Cancel &cancel=*static_cast<Cancel *>(opaque);
    ++cancel.calls;
    return done==0;
}
void word() {
    rt::Bytes original;
    if (!fixture("fixture.rword",original)) return;
    rw::Text error, encoded;
    rw::Document document;
    if (!rw::decode_document(original.data(),original.size(),rw::FileFormat::Native,document,error)) {
        check(false,"Word original Unix fixture loads"); return;
    }
    check(rw::encode_document(document,rw::FileFormat::Native,encoded,error) &&
        equal(encoded.c_str(),encoded.size(),original.data(),original.size()),"Word original bytes preserved");
    char file[4096]; path(file,directory,"文字-📝.rword"); ro::unlink(file);
    rw::FileStamp missing, saved;
    check(rw::file_stamp(file,missing,error) && !missing.exists,"Word absent stamp");
    check(rw::save_document(file,document,rw::FileFormat::Native,error,&missing),"Word Unicode atomic save");
    check(on_disk(file,original.data(),original.size()),"Word exact persisted bytes");
    rw::Document loaded;
    check(rw::load_document(file,rw::FileFormat::Native,loaded,error),"Word Unicode load");
    check(rw::file_stamp(file,saved,error) && saved.exists,"Word saved identity");
    Cancel cancel;
    check(!rw::save_document(file,document,rw::FileFormat::Native,error,&saved,cancel_after_progress,&cancel) &&
        cancel.calls>1,"Word cancellation reaches real save pipeline");
    check(on_disk(file,original.data(),original.size()),"Word cancellation preserves original");
    static const char external[]="external Word writer\n";
    check(write_bytes(file,external,sizeof(external)-1),"Word external modification");
    check(!rw::save_document(file,document,rw::FileFormat::Native,error,&saved),"Word stale stamp rejected");
    check(on_disk(file,external,sizeof(external)-1),"Word conflict preserves other writer");
    check(write_bytes(file,original.data(),original.size()),"Word restore test fixture");
    malformed(original,[&](const char *bytes,size_t size) {
        bool accepted=rw::decode_document(bytes,size,rw::FileFormat::Native,document,error);
        rw::Text after;
        check(rw::encode_document(document,rw::FileFormat::Native,after,error) &&
            equal(after.c_str(),after.size(),original.data(),original.size()),"Word failed decode preserves model");
        return accepted;
    });
}
void excel() {
    rt::Bytes original;
    if (!fixture("fixture.rexl",original)) return;
    char source[4096],file[4096],badfile[4096];
    path(source,fixtures,"fixture.rexl"); path(file,directory,"表格-📊.rexl"); path(badfile,directory,"truncated.rexl");
    ro::unlink(file);
    rx::Text error;
    rx::Workbook book;
    if (!rx::load_workbook(source,book,error)) { check(false,"Excel original Unix fixture loads"); return; }
    rx::FileStamp missing,saved;
    check(rx::file_stamp(file,missing,error) && !missing.exists,"Excel absent stamp");
    check(rx::save_workbook(file,book,error,&missing),"Excel Unicode atomic save");
    check(on_disk(file,original.data(),original.size()),"Excel original bytes preserved");
    rx::Workbook loaded;
    check(rx::load_workbook(file,loaded,error),"Excel Unicode load");
    check(rx::file_stamp(file,saved,error) && saved.exists,"Excel saved identity");
    Cancel cancel;
    check(!rx::save_workbook(file,book,error,&saved,cancel_after_progress,&cancel) && cancel.calls>1,
        "Excel cancellation reaches real save pipeline");
    check(on_disk(file,original.data(),original.size()),"Excel cancellation preserves original");
    static const char external[]="external Excel writer\n";
    check(write_bytes(file,external,sizeof(external)-1),"Excel external modification");
    check(!rx::save_workbook(file,book,error,&saved),"Excel stale stamp rejected");
    check(on_disk(file,external,sizeof(external)-1),"Excel conflict preserves other writer");
    check(write_bytes(file,original.data(),original.size()),"Excel restore test fixture");
    malformed(original,[&](const char *bytes,size_t size) {
        check(write_bytes(badfile,bytes,size),"Excel malformed file write");
        bool accepted=rx::load_workbook(badfile,book,error);
        bool saved_model=rx::save_workbook(file,book,error);
        if (!saved_model) fprintf(stderr,"Excel failed-load save: %s\n",error.c_str());
        check(saved_model,"Excel failed-load model can be saved");
        check(on_disk(file,original.data(),original.size()),"Excel failed load preserves model bytes");
        return accepted;
    });
    ro::unlink(badfile);
}
void powerpoint() {
    rt::Bytes original;
    if (!fixture("fixture.rppt",original)) return;
    rp::Deck deck;
    rp::Text error,encoded;
    if (!rp::decode_deck(original.data(),original.size(),deck,error)) { check(false,"PPT original Unix fixture loads"); return; }
    check(rp::encode_deck(deck,encoded,error) && equal(encoded.c_str(),encoded.size(),original.data(),original.size()),
        "PPT original bytes preserved");
    char file[4096]; path(file,directory,"演示-🎞.rppt"); ro::unlink(file);
    rp::FileStamp missing,saved;
    check(rp::file_stamp(file,missing,error) && !missing.exists,"PPT absent stamp");
    check(rp::save_deck(file,deck,error,&missing),"PPT Unicode atomic save");
    check(on_disk(file,original.data(),original.size()),"PPT persisted bytes");
    rp::Deck loaded;
    check(rp::load_deck(file,loaded,error),"PPT Unicode load");
    check(rp::file_stamp(file,saved,error) && saved.exists,"PPT saved identity");
    static const char external[]="external PPT writer\n";
    check(write_bytes(file,external,sizeof(external)-1),"PPT external modification");
    check(!rp::save_deck(file,deck,error,&saved),"PPT stale stamp rejected");
    check(on_disk(file,external,sizeof(external)-1),"PPT conflict preserves other writer");
    check(write_bytes(file,original.data(),original.size()),"PPT restore test fixture");
    malformed(original,[&](const char *bytes,size_t size) {
        bool accepted=rp::decode_deck(bytes,size,deck,error);
        rp::Text after;
        check(rp::encode_deck(deck,after,error) && equal(after.c_str(),after.size(),original.data(),original.size()),
            "PPT failed decode preserves model");
        return accepted;
    });
}
bool finish(rt::IoJob &job,rt::IoJob::Reply &reply) {
    double deadline=ro::monotonic()+20;
    while (!job.poll()) if (ro::monotonic()>deadline) return false;
    reply=job.take();
    return true;
}
template<class Decode,class Encode>
void shared_io(const char *name,const char *unicode,Decode decode,Encode encode) {
    rt::Bytes original;
    if (!fixture(name,original)) return;
    check(decode(original.data(),original.size()),"shared IO original fixture decode");
    rt::Bytes encoded;
    check(encode(encoded) && equal(encoded.data(),encoded.size(),original.data(),original.size()),
        "shared IO original bytes preserved");
    char file[4096]; path(file,directory,unicode); ro::unlink(file);
    rt::IoJob job;
    rt::Text error;
    rt::FileStamp missing;
    rt::IoJob::Reply reply;
    check(job.write(file,original,missing,error),"background Unicode save starts");
    check(!job.write(file,original,missing,error),"one job owns request/result lifetime");
    check(finish(job,reply) && reply.ok && reply.stamp.exists,"background Unicode save completes");
    check(!job.busy() && on_disk(file,original.data(),original.size()),"background save exact persisted bytes");
    rt::FileStamp saved=reply.stamp;
    check(job.read(file,error) && finish(job,reply) && reply.ok &&
        equal(reply.bytes.data(),reply.bytes.size(),original.data(),original.size()),"background Unicode read roundtrip");
    check(job.probe(file,error,file) && finish(job,reply) && !reply.ok,"export rejects active document identity");
    static const char external[]="external shared writer\n";
    check(write_bytes(file,external,sizeof(external)-1),"shared IO external modification");
    check(job.write(file,original,saved,error) && finish(job,reply) && !reply.ok,"background stale stamp rejected");
    check(on_disk(file,external,sizeof(external)-1),"background conflict preserves other writer");
    check(write_bytes(file,original.data(),original.size()),"shared IO restore test fixture");
    malformed(original,[&](const char *bytes,size_t size) {
        bool accepted=decode(bytes,size);
        rt::Bytes after;
        check(encode(after) && equal(after.data(),after.size(),original.data(),original.size()),
            "shared IO failed decode preserves model");
        return accepted;
    });
}
template<class Text,class Reserve,class Lock,class Unlock>
void recovery(Reserve reserve,Lock lock,Unlock unlock) {
    Text file,error;
    if (!reserve(file,error)) { fprintf(stderr,"Recovery: %s\n",error.c_str()); check(false,"reserve real per-user recovery"); return; }
    check(ro::absolute_path(file.c_str()),"recovery uses absolute per-user path");
    ro::Stat st;
    check(ro::lstat(file.c_str(),&st)==0 && S_ISREG(st.st_mode) && st.st_uid==ro::geteuid() &&
        (st.st_mode&0777)==0600 && st.st_nlink==1,"recovery file owner, private ACL/mode and identity");
    int owner=-1,other=-1;
    check(lock(file.c_str(),owner,error) && owner>=0,"recovery lock acquired");
    check(!lock(file.c_str(),other,error) && other<0,"second recovery owner excluded");
    unlock(owner);
    check(owner<0 && lock(file.c_str(),other,error),"recovery lock released on close");
    unlock(other);
    check(ro::unlink(file.c_str())==0,"recovery file cleanup");
    char side[4096]; snprintf(side,sizeof(side),"%s.lock",file.c_str());
    check(ro::unlink(side)==0,"recovery lock cleanup");
}
struct ConcurrentSave {
    std::atomic<unsigned> *ready;
    std::atomic<bool> *go;
    const char *path;
    const rt::Bytes *bytes;
    rt::FileStamp expected,out;
    rt::Text error;
    bool ok=false;
};
void simultaneous_save(void *opaque) {
    ConcurrentSave &save=*static_cast<ConcurrentSave *>(opaque);
    save.ready->fetch_add(1,std::memory_order_release);
    while (!save.go->load(std::memory_order_acquire)) {}
    save.ok=rt::atomic_save(save.path,*save.bytes,save.expected,save.out,save.error);
}
template<class Text,class Lock,class Unlock>
void public_recovery_lock(const char *name,Lock lock,Unlock unlock,bool security_query) {
    char file[4096],side[4096];
    path(file,directory,name); snprintf(side,sizeof(side),"%s.lock",file);
    ro::unlink(file); ro::unlink(side);
    check(write_bytes(file,"public recovery probe",21),"public-volume recovery test file created");
    Text error;
    int descriptor=-1;
    errno=0;
    bool refused=!lock(file,descriptor,error) && descriptor<0;
    int failure=errno;
    check(refused,security_query?"recovery lock rejects storage without private ownership":
        "PPT recovery refuses a file outside its private recovery directory");
    if (security_query) check(failure==ENOSYS,"recovery rejection comes from unsupported filesystem security");
    unlock(descriptor);
    check(on_disk(file,"public recovery probe",21),"refused recovery lock preserves file bytes");
    ro::unlink(side);
    check(ro::unlink(file)==0,"refused recovery lock file cleanup");
}
void public_permissions() {
    char file[4096]; path(file,directory,"公开存储权限.tmp"); ro::unlink(file);
    int fd=ro::open(file,O_RDWR|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600);
    check(fd>=0,"public-volume permission test file created");
    if (fd>=0) {
        ro::Stat st;
        check(ro::set_document_mode(fd,0600)==0,"ordinary document accepts storage without ACLs");
        check(ro::fstat(fd,&st)==0 && S_ISREG(st.st_mode) && uint64_t(st.st_uid)==UINT64_MAX &&
            (st.st_mode&0777)==0666,"storage without ACLs reports unknown owner and public mode");
        check(ro::fchmod(fd,0600)<0,"strict file privacy rejects storage without ACLs");
        check(ro::fstat(fd,&st)==0 && uint64_t(st.st_uid)==UINT64_MAX && (st.st_mode&0777)==0666,
            "failed strict privacy never reports private permissions");
        check(ro::close(fd)==0 && ro::unlink(file)==0,"public-volume permission file cleanup");
    }
    fd=ro::open(directory,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
    check(fd>=0,"public-volume directory opened");
    if (fd>=0) {
        ro::Stat st;
        check(ro::fchmod(fd,0700)<0,"strict directory privacy rejects storage without ACLs");
        check(ro::fstat(fd,&st)==0 && S_ISDIR(st.st_mode) && uint64_t(st.st_uid)==UINT64_MAX &&
            (st.st_mode&0777)==0777,"public directory never reports private owner or permissions");
        ro::close(fd);
    }
    public_recovery_lock<rw::Text>("public-recovery.rword",rw::lock_recovery_path,rw::unlock_recovery_path,true);
    public_recovery_lock<rx::Text>("public-recovery.rexl",rx::lock_recovery_path,rx::unlock_recovery_path,true);
    public_recovery_lock<rp::Text>("public-recovery.rppt",rp::lock_recovery_path,rp::unlock_recovery_path,false);
}
void files_and_races() {
    char first[4096],second[4096],lockfile[4096];
    path(first,directory,"原子-α.tmp"); path(second,directory,"原子-β.tmp"); path(lockfile,directory,"exclusive.lock");
    check(write_bytes(first,"first",5) && write_bytes(second,"second",6),"no-replace setup");
    {
        ro::SaveLock holder,same,independent,alias_lock;
        check(holder.acquire(first),"target save lock acquired without waiting");
        check(!same.acquire(first) && (errno==EAGAIN || errno==EWOULDBLOCK),"same target save excluded");
        check(independent.acquire(second),"different target save remains available");
        check(on_disk(first,"first",5),"identity save lock permits normal document reads");
        if (!public_volume) {
            check(hardlink(first,lockfile),"save target hardlink alias created");
            check(!alias_lock.acquire(lockfile) && (errno==EAGAIN || errno==EWOULDBLOCK),"hardlink identity save excluded");
            check(ro::unlink(lockfile)==0,"save alias cleanup");
        }
    }
    {
        ro::SaveLock released;
        check(released.acquire(first),"save ownership released after failed and successful acquisitions");
    }
    char replacement[4096]; path(replacement,directory,"replace-locked-target.tmp");
    check(write_bytes(replacement,"first",5),"locked-target replacement setup");
    {
        ro::SaveLock held;
        check(held.acquire(first),"replacement retains save transaction ownership");
        check(ro::rename(replacement,first)==0,"save identity ownership permits target replacement");
        check(on_disk(first,"first",5),"locked-target replacement persists complete bytes");
    }
    ro::unlink(replacement);
#ifdef _WIN32
    if (!public_volume) {
        bool linked=hardlink(first,lockfile);
        check(linked,"cross-process identity alias created");
        if (linked) {
            ChildLock child;
            bool started=child.start(first,false);
            check(started,"another process owns existing target identity");
            if (started) {
                ro::SaveLock alias,independent;
                check(!alias.acquire(lockfile) && (errno==EAGAIN || errno==EWOULDBLOCK),
                    "another process excludes hardlink alias");
                check(independent.acquire(second),"another process leaves different targets available");
            }
            check(child.finish(),"cross-process identity owner exits cleanly");
            {
                ro::SaveLock alias;
                check(alias.acquire(lockfile),"cross-process identity release permits alias");
            }
            ChildLock abandoned;
            started=abandoned.start(first,true);
            check(started,"terminated owner leaves abandoned identity in live process");
            if (started) {
                ro::SaveLock alias;
                check(alias.acquire(lockfile),"abandoned identity transfers ownership");
                check(on_disk(lockfile,"first",5),"abandoned ownership preserves original bytes");
            }
            check(abandoned.finish(),"abandoned identity process cleanup");
            check(ro::unlink(lockfile)==0,"cross-process identity alias cleanup");
        }
    }
#endif
    check(ro::rename_noreplace(first,second)<0 && errno==EEXIST,"no-replace conflict status");
    check(on_disk(first,"first",5) && on_disk(second,"second",6),"no-replace preserves both files");
    check(ro::unlink(second)==0 && ro::rename_noreplace(first,second)==0,"no-replace atomic commit");
    ro::Stat st;
    check(ro::lstat(first,&st)<0 && errno==ENOENT && on_disk(second,"first",5),"no-replace removes source once");
    ro::unlink(second);
    int a=ro::open(lockfile,O_RDWR|O_CREAT|O_CLOEXEC|O_NOFOLLOW,0600);
    int b=ro::open(lockfile,O_RDWR|O_CLOEXEC|O_NOFOLLOW);
    check(a>=0 && b>=0 && ro::flock(a,LOCK_EX|LOCK_NB)==0,"native independent descriptor lock");
    check(ro::flock(b,LOCK_EX|LOCK_NB)<0 && (errno==EAGAIN || errno==EWOULDBLOCK),"native competing lock blocked");
    ro::close(a);
    check(ro::flock(b,LOCK_EX|LOCK_NB)==0,"native lock lifetime ends on close");
    ro::close(b); ro::unlink(lockfile);
    rt::Bytes original;
    if (fixture("fixture.rtodo",original)) {
        char target[4096]; path(target,directory,"并行保存.rtodo"); ro::unlink(target);
        std::atomic<unsigned> ready{0}; std::atomic<bool> go{false};
        ConcurrentSave one{&ready,&go,target,&original,{}, {},{},false};
        ConcurrentSave two{&ready,&go,target,&original,{}, {},{},false};
        ro::Worker w1,w2;
        bool started1=w1.start(simultaneous_save,&one), started2=w2.start(simultaneous_save,&two);
        if (started1 && started2) {
            while (ready.load(std::memory_order_acquire)!=2) {}
            go.store(true,std::memory_order_release); w1.wait(); w2.wait();
            check(unsigned(one.ok)+unsigned(two.ok)==1,"two stale concurrent saves have exactly one winner");
            check(on_disk(target,original.data(),original.size()),"concurrent save commits complete native bytes");
        } else { go.store(true,std::memory_order_release); check(false,"concurrent workers start"); }
        w1.wait(); w2.wait(); ro::unlink(target);
    }
    int fd=ro::open(directory,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
    int copy=fd>=0?ro::dup(fd):-1;
    if (fd>=0) ro::close(fd);
    ro::Dir *dir=copy>=0?ro::fdopendir(copy):nullptr;
    bool seen=false,temporary=false;
    if (dir) {
        while (ro::Dirent *entry=ro::readdir(dir)) {
            if (!strcmp(entry->d_name,"文字-📝.rword")) seen=true;
            if (strstr(entry->d_name,".tmp.") || strstr(entry->d_name,".rollback.") ||
                (strstr(entry->d_name,".relieftodo-") && strstr(entry->d_name,".tmp"))) temporary=true;
        }
        check(ro::closedir(dir)==0,"directory iterator owns duplicated descriptor");
    } else if (copy>=0) ro::close(copy);
    check(seen,"directory enumeration returns UTF-8 names");
    check(!temporary,"completed and cancelled saves clean temporary/rollback files");
#ifdef _WIN32
    if (directory[0] && directory[1]==':') {
        char root_file[128],slash_file[128],record[256];
        snprintf(root_file,sizeof(root_file),"%c:\\ReliefOffice-drive-root-test.tmp",directory[0]);
        snprintf(slash_file,sizeof(slash_file),"/%c:/ReliefOffice-drive-root-test.tmp",directory[0]);
        snprintf(record,sizeof(record),"%c:\\.reliefoffice-locks\\ReliefOffice-drive-root-test.tmp",directory[0]);
        {
            ro::SaveLock root_owner,alias;
            check(root_owner.acquire(root_file),"native drive-root save lock");
            check(ro::lstat(record,&st)==0 && S_ISREG(st.st_mode),"drive-root lock uses volume root directory");
            check(!alias.acquire(slash_file) && (errno==EAGAIN || errno==EWOULDBLOCK),"slash-prefixed drive root aliases native root");
        }
    }
#endif
}

}
int main(int argc,char **argv) {
#ifdef _WIN32
    if (argc==5 && (!strcmp(argv[1],"--hold-lock-child") || !strcmp(argv[1],"--abandon-lock-child")))
        return lock_child(argv[2],argv[3],argv[4],!strcmp(argv[1],"--abandon-lock-child"));
#endif
    if (argc==4 && !strcmp(argv[3],"--public-volume")) public_volume=true;
    else if (argc!=3) { fprintf(stderr,"Usage: office_io <original-fixtures> <existing-work-directory> [--public-volume]\n"); return 2; }
    fixtures=argv[1]; directory=argv[2];
    if (!existing_directory(fixtures,"fixtures") || !existing_directory(directory,"work")) return 2;
    word(); excel(); powerpoint();
    rt::Document todo; rt::Text todo_error;
    shared_io("fixture.rtodo","任务-✅.rtodo",
        [&](const char *p,size_t n){return rt::decode_document(p,n,todo,todo_error);},
        [&](rt::Bytes &out){return rt::encode_document(todo,out,todo_error);});
    rd::Document diagram; rd::Text diagram_error;
    shared_io("fixture.rdiagram","图表-🗺.rdiagram",
        [&](const char *p,size_t n){return rd::decode(p,n,diagram,diagram_error);},
        [&](rt::Bytes &out){rd::Bytes bytes; bool ok=rd::encode(diagram,bytes,diagram_error);
            if (!ok || !out.resize(bytes.size())) return false;
            memcpy(out.data(),bytes.data(),bytes.size()); return true;});
    rmp::Document mathpp; rmp::Text mathpp_error;
    rmp::Expression expression;
    check(!expression.valid(),"uncompiled worker expression rejected");
    check(expression.compile("if(x>0,sin(x)+max(a,b),-x^2)",mathpp_error) && expression.valid(),
        "compiled worker expression validates stack and variables");
    check(!expression.compile("sin(",mathpp_error) && !expression.valid(),
        "failed compilation invalidates worker expression");
    shared_io("fixture.rmathpp","几何-📐.rmathpp",
        [&](const char *p,size_t n){return rmp::decode(p,n,mathpp,mathpp_error);},
        [&](rt::Bytes &out){rmp::Bytes bytes; bool ok=rmp::encode(mathpp,bytes,mathpp_error);
            if (!ok || !out.resize(bytes.size())) return false;
            memcpy(out.data(),bytes.data(),bytes.size()); return true;});
    if (public_volume) public_permissions();
    else {
        recovery<rw::Text>(rw::reserve_recovery_path,rw::lock_recovery_path,rw::unlock_recovery_path);
        recovery<rx::Text>(rx::reserve_recovery_path,rx::lock_recovery_path,rx::unlock_recovery_path);
        recovery<rp::Text>(rp::reserve_recovery_path,rp::lock_recovery_path,rp::unlock_recovery_path);
    }
    files_and_races();
    printf("ReliefOffice native IO: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
