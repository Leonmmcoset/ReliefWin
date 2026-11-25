/*
 * PROJECT:     ReliefOffice API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Pause the real final rename to reproduce and pin save conflicts
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include <platform/posix.h>
#include <platform/worker.h>
#include <reliefword/document.h>
#include <reliefexcel/document.h>
#include <reliefppt/document.h>
#include <relieftodo/file_io.h>
#include <atomic>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

namespace {
std::atomic<bool> intercepted{false},release_rename{false};
std::atomic<unsigned> armed{0};
std::atomic<bool> active{false};
unsigned checks=0,failures=0;
const char *work=".";
void check(bool result,const char *name) {
    ++checks;
    if (!result) { ++failures; fprintf(stderr,"FAIL %s\n",name); }
}
void intercept() {
    if (!active.load(std::memory_order_acquire)) return;
    if (armed.fetch_add(1,std::memory_order_acq_rel)!=0) return;
    intercepted.store(true,std::memory_order_release);
    while (!release_rename.load(std::memory_order_acquire)) {}
}
void file(char (&path)[4096],const char *component) {
    int length=snprintf(path,sizeof(path),"%s/%s",work,component);
    check(length>0 && size_t(length)<sizeof(path),"path fits");
}
struct Attempt {
    void (*save)(Attempt &);
    const char *path;
    void *document;
    void *expected;
    bool ok=false;
};
void perform(void *opaque) { Attempt &attempt=*static_cast<Attempt *>(opaque); attempt.save(attempt); }
void race(Attempt first,Attempt second,bool independent) {
    intercepted.store(false,std::memory_order_release);
    release_rename.store(false,std::memory_order_release);
    armed.store(0,std::memory_order_release);
    active.store(true,std::memory_order_release);
    ro::Worker writer;
    if (!writer.start(perform,&first)) { check(false,"start first writer"); return; }
    double deadline=ro::monotonic()+10;
    while (!intercepted.load(std::memory_order_acquire) && !writer.finished() && ro::monotonic()<deadline) {}
    bool paused=intercepted.load(std::memory_order_acquire);
    check(paused,"first writer reaches final rename after all stamp checks");
    if (paused) second.save(second);
    release_rename.store(true,std::memory_order_release);
    writer.wait();
    active.store(false,std::memory_order_release);
    check(first.ok,"first writer commits complete document");
    check(second.ok==independent,independent?"different file saves while first rename is paused":"same identity writer rejected before rename");
}
void word() {
    rw::Editor first,second;
    check(first.insert_utf8("writer A",8) && second.insert_utf8("writer B",8),"Word models");
    rw::Text error;
    char path[4096],other[4096],alias[4096];
    file(path,"race.rword"); file(other,"independent.rword"); file(alias,"alias.rword");
    rw::FileStamp expected,missing;
    auto save=[](Attempt &attempt) {
        rw::Text reason;
        attempt.ok=rw::save_document(attempt.path,*static_cast<rw::Document *>(attempt.document),
            rw::FileFormat::Native,reason,static_cast<rw::FileStamp *>(attempt.expected));
    };
    ro::unlink(path); ro::unlink(other); ro::unlink(alias);
    race({save,path,&first.doc,&missing},{save,path,&second.doc,&missing},false);
    check(rw::file_stamp(path,expected,error),"Word current identity");
    check(::link(path,alias)==0,"Word hardlink alias");
    check(rw::file_stamp(path,expected,error),"Word identity after hardlink creation");
    race({save,path,&first.doc,&expected},{save,alias,&second.doc,&expected},false);
    ro::unlink(alias);
    check(rw::file_stamp(path,expected,error),"Word refreshed identity");
    race({save,path,&first.doc,&expected},{save,other,&second.doc,&missing},true);
    ro::unlink(path); ro::unlink(other);
}
void excel() {
    rx::Workbook first,second;
    check(first.init() && second.init(),"Excel models");
    check(first.set_input(first.active_id,0,0,"writer A") && second.set_input(second.active_id,0,0,"writer B"),"Excel distinct contents");
    rx::Text error;
    char path[4096],other[4096],alias[4096];
    file(path,"race.rexl"); file(other,"independent.rexl"); file(alias,"alias.rexl");
    rx::FileStamp expected,missing;
    auto save=[](Attempt &attempt) {
        rx::Text reason;
        attempt.ok=rx::save_workbook(attempt.path,*static_cast<rx::Workbook *>(attempt.document),reason,
            static_cast<rx::FileStamp *>(attempt.expected));
    };
    ro::unlink(path); ro::unlink(other); ro::unlink(alias);
    race({save,path,&first,&missing},{save,path,&second,&missing},false);
    check(rx::file_stamp(path,expected,error),"Excel current identity");
    check(::link(path,alias)==0,"Excel hardlink alias");
    check(rx::file_stamp(path,expected,error),"Excel identity after hardlink creation");
    race({save,path,&first,&expected},{save,alias,&second,&expected},false);
    ro::unlink(alias);
    check(rx::file_stamp(path,expected,error),"Excel refreshed identity");
    race({save,path,&first,&expected},{save,other,&second,&missing},true);
    ro::unlink(path); ro::unlink(other);
}
void powerpoint() {
    rp::Deck first,second;
    check(first.init() && second.init() && first.title.set("writer A") && second.title.set("writer B"),"PPT models");
    rp::Text error;
    char path[4096],other[4096],alias[4096];
    file(path,"race.rppt"); file(other,"independent.rppt"); file(alias,"alias.rppt");
    rp::FileStamp expected,missing;
    auto save=[](Attempt &attempt) {
        rp::Text reason;
        attempt.ok=rp::save_deck(attempt.path,*static_cast<rp::Deck *>(attempt.document),reason,
            static_cast<rp::FileStamp *>(attempt.expected));
    };
    ro::unlink(path); ro::unlink(other); ro::unlink(alias);
    check(rp::save_deck(path,first,error) && rp::file_stamp(path,expected,error),"PPT existing save target");
    race({save,path,&first,&expected},{save,path,&second,&expected},false);
    check(rp::file_stamp(path,expected,error),"PPT current identity");
    check(::link(path,alias)==0,"PPT hardlink alias");
    check(rp::file_stamp(path,expected,error),"PPT identity after hardlink creation");
    race({save,path,&first,&expected},{save,alias,&second,&expected},false);
    ro::unlink(alias);
    check(rp::file_stamp(path,expected,error),"PPT refreshed identity");
    race({save,path,&first,&expected},{save,other,&second,&missing},true);
    ro::unlink(path); ro::unlink(other);
}
void shared_directory() {
    if (::geteuid()!=0) return;
    const char root[]="/tmp/reliefoffice-shared-save-tests";
    char first[4096],second[4096];
    check(::mkdir(root,0777)==0 || errno==EEXIST,"shared destination directory");
    struct stat root_info;
    if (::lstat(root,&root_info)<0 || !S_ISDIR(root_info.st_mode) || root_info.st_uid!=0) {
        check(false,"shared test directory is owned by this root test process"); return;
    }
    check(::chmod(root,0777)==0,"shared directory write permissions");
    snprintf(first,sizeof(first),"%s/first",root);
    snprintf(second,sizeof(second),"%s/second",root);
    char record[4096];
    snprintf(record,sizeof(record),"%s/.reliefoffice-locks/first",root); ::unlink(record);
    snprintf(record,sizeof(record),"%s/.reliefoffice-locks/second",root); ::unlink(record);
    auto child=[&](bool conflict) {
        pid_t pid=::fork();
        if (pid==0) {
            if (::setgid(65534)<0 || ::setuid(65534)<0) _exit(10);
            ro::SaveLock same,independent;
            bool acquired=same.acquire(first);
            bool ok=acquired!=conflict;
            if (conflict) ok=ok && (errno==EAGAIN || errno==EWOULDBLOCK);
            ok=ok && independent.acquire(second);
            _exit(ok?0:11);
        }
        int status=0;
        check(pid>0 && ::waitpid(pid,&status,0)==pid && WIFEXITED(status) && WEXITSTATUS(status)==0,
            "different authorized user respects target lock and saves independent file");
    };
    {
        ro::SaveLock owner;
        check(owner.acquire(first),"first user owns one shared target");
        child(true);
    }
    child(false);
}
void cross_component() {
    rw::Editor word;
    check(word.insert_utf8("Word writer",11),"cross-component Word document");
    rt::Bytes bytes;
    check(bytes.resize(11),"cross-component shared IO bytes");
    memcpy(bytes.data(),"Todo writer",11);
    auto save_word=[](Attempt &attempt) {
        rw::Text error;
        attempt.ok=rw::save_document(attempt.path,*static_cast<rw::Document *>(attempt.document),
            rw::FileFormat::Native,error,static_cast<rw::FileStamp *>(attempt.expected));
    };
    auto save_todo=[](Attempt &attempt) {
        rt::Text error; rt::FileStamp output;
        attempt.ok=rt::atomic_save(attempt.path,*static_cast<rt::Bytes *>(attempt.document),
            *static_cast<rt::FileStamp *>(attempt.expected),output,error);
    };
    char path[4096]; file(path,"cross-component.rword");
    rw::FileStamp word_missing; rt::FileStamp todo_missing;
    ro::unlink(path);
    race({save_word,path,&word.doc,&word_missing},{save_todo,path,&bytes,&todo_missing},false);
    ro::unlink(path);
    race({save_todo,path,&bytes,&todo_missing},{save_word,path,&word.doc,&word_missing},false);
    ro::unlink(path);
}
void special_files() {
    char path[4096],record[4096];
    file(path,"save-fifo");
    ro::unlink(path);
    check(::mkfifo(path,0600)==0,"FIFO save target created");
    auto bounded_rejection=[&](const char *target) {
        pid_t child=::fork();
        if (child==0) { ro::SaveLock guard; _exit(guard.acquire(target)?1:0); }
        int status=0; pid_t finished=0;
        double deadline=ro::monotonic()+2;
        while (child>0 && !(finished=::waitpid(child,&status,WNOHANG)) && ro::monotonic()<deadline) ::usleep(1000);
        if (child>0 && !finished) { ::kill(child,SIGKILL); ::waitpid(child,&status,0); }
        check(child>0 && finished==child && WIFEXITED(status) && WEXITSTATUS(status)==0,
            "FIFO target or lock record rejected without waiting for a writer");
    };
    bounded_rejection(path);
    ro::unlink(path);
    snprintf(record,sizeof(record),"%s/.reliefoffice-locks/save-fifo",work);
    ro::unlink(record);
    check(::mkfifo(record,0600)==0,"FIFO lock record created");
    bounded_rejection(path);
    ro::unlink(record);
}
}
extern "C" int __real_rename(const char *,const char *);
extern "C" int __real_renameat(int,const char *,int,const char *);
extern "C" int __wrap_rename(const char *from,const char *to) {
    intercept(); return __real_rename(from,to);
}
extern "C" int __wrap_renameat(int from_dir,const char *from,int to_dir,const char *to) {
    intercept(); return __real_renameat(from_dir,from,to_dir,to);
}
int main(int argc,char **argv) {
    if (argc!=2) return 2;
    work=argv[1];
    word(); excel(); powerpoint(); cross_component(); shared_directory(); special_files();
    printf("ReliefOffice final rename conflicts: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
