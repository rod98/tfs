#include "config.h"
#include "params.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include "defs.h"
#include "filetable.h"
#include "file_io.h"
#include "dir.h"


#ifdef HAVE_SYS_XATTR_H
#include <sys/xattr.h>
#endif

#include "log.h"

#define TFS_DATA ((struct _tfs_state *) fuse_get_context()->private_data)

tfs_state *tdata;

int tfs_inner_getattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi);

uint8_t check_flag(uint64_t flags, uint64_t flag) {
	return (flags & flag) ? (1) : (0);
}

int tfs_getattr(const char *path, struct stat *statbuf) {
	log_msg("%s\n", __FUNCTION__);
	return tfs_inner_getattr(path, statbuf, NULL);
}

int tfs_inner_getattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi)
{
	int  retstat = 0;
	
	log_msg("%s {\n", __FUNCTION__);
	
	log_msg("\t%s\n", path);
	
	if (statbuf && path) {
		
		if (!strcmp(path, "/"))
			statbuf->st_mode = S_IFDIR;
		else {
			statbuf->st_mode = S_IFREG; 
			path = get_last_file(path);
		}
	}
	
	if (fi) {
		log_fi(fi);		
		ftbl_infstat(tdata, ftbl_get_findex(tdata, path), NULL, statbuf);
	}
	else if (strcmp(path, "/")) {
		uint64_t ind = ftbl_get_findex(tdata, path);
		log_msg("\tgot ind: %llu\n", ind);
		retstat = ftbl_infstat(tdata, ind, NULL, statbuf);
		log_msg("retstat: %d\n", retstat);	
	}
		
	log_msg("}\n");

	return retstat;
}

int tfs_readlink(const char *path, char *link, size_t size)
{
	int retstat = 0;
	
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	
	return retstat;
}

int tfs_open(const char *path, struct fuse_file_info *fi);
int tfs_release(const char *path, struct fuse_file_info *fi);
int tfs_mknod(const char *path, mode_t mode, dev_t dev)
{
	int retstat = 0;
	
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	struct fuse_file_info fi;
	fi.flags = O_WRONLY;
	tfs_open(path, &fi);
	tfs_release(path, &fi);
	
	log_msg("}\n");
	
	return retstat;
}

/** Create a directory */
int tfs_mkdir(const char *path, mode_t mode)
{
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	ftbl_newfile (tdata, path, ftbl_getfcnt(tdata), 1);
	
	log_msg("}\n");
	return 0;
}

/** Remove a file */
int tfs_unlink(const char *path)
{
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}

/** Remove a directory */
int tfs_rmdir(const char *path)
{
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}

int tfs_symlink(const char *path, const char *link)
{
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}

int tfs_rename(const char *path, const char *newpath)
{
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}

/** Create a hard link to a file */
int tfs_link(const char *path, const char *newpath)
{
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}

/** Change the permission bits of a file */
int tfs_chmod(const char *path, mode_t mode)
{

	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}

/** Change the owner and group of a file */
int tfs_chown(const char *path, uid_t uid, gid_t gid)
  
{
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}

/** Change the size of a file */
int tfs_truncate(const char *path, off_t newsize)
{

	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}

int tfs_utime(const char *path, struct utimbuf *ubuf)
{
	uint64_t pos;
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
		
	pos = ftbl_devpos(tdata, ftbl_get_findex(tdata, path)) + FTBL_RTIME_OFST;
	pwrite(TFS_DATA->devdesc, &(ubuf->modtime), sizeof(ubuf->modtime), pos);
	
	log_msg("}\n");
	return 0;
}

int tfs_open(const char *path, struct fuse_file_info *fi)
{
	uint64_t ind;
	int      err = 0;
	
	log_msg("%s() {\n", __FUNCTION__);
	char *short_path = get_last_file(path);
	log_msg("\t Last file: %s\n", path);
	
	ind = ftbl_get_findex(tdata, short_path);
	
	log_fi(fi);
	fi->fh = 0;
	
	
	if (ind == ftbl_getfcnt(tdata)) {
		if (check_flag(fi->flags, O_WRONLY) || check_flag(fi->flags, O_RDWR)) {
			ftbl_newfile(tdata, short_path, ind, 0);

			ftbl_setfsz(tdata, ind, 0);
		}
		else {
			log_msg("\tWell the file %s ain't here\n", short_path);

			err = -2;
		}
	}
	char dirname[FNAME_MAXNAME+1];
	char *npath = path;
	
	log_msg("full path is: %s\n", npath);
	while ((npath = get_next_dir(npath, dirname))) {
		log_msg("npath: %s\n", npath);
		if (strcmp(dirname, short_path)) {
			log_msg("DIRNAME: %s\n", dirname);
			
			// now we need to write to the directory
			uint64_t dir_ind = ftbl_get_findex(tdata, dirname);
			log_msg("dir_ind: %llu\n", dir_ind);
			if (check_flag(fi->flags, O_WRONLY) || check_flag(fi->flags, O_RDWR))
				dir_append(tdata, dir_ind, ind);
		}
	}
	fi->fh = ind;
	
	log_msg("}\n");
	
	return err;
}


int tfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	uint64_t start = ftbl_read_start(tdata, fi->fh);

	log_msg("%s {\n",  __FUNCTION__);

	log_msg(
		"\ntfs_read(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
		path, buf, size, offset, fi
	);

	log_fi(fi);
	
	memset(buf, 'x', size);
	
	read_block(tdata, start, (uint8_t *)buf);
	
	log_msg("}\n");
	
	return ftbl_getfsz(tdata, fi->fh);
}


int tfs_write(
	const char *path, const char *buf, size_t size, off_t offset,
	struct fuse_file_info *fi)
{

	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	log_msg("\t%d\n", size);
	
	uint64_t blck = ftbl_read_start(tdata, fi->fh);
	
	write_block(tdata, blck, (uint8_t *)buf);
	ftbl_setfsz(tdata, fi->fh, size);
	
	log_msg("}\n");
	
	return size;
}

int tfs_statfs(const char *path, struct statvfs *statv)
{
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}


int tfs_flush(const char *path, struct fuse_file_info *fi)
{
	//log_fi(fi);
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
	
	return 0;
}


int tfs_release(const char *path, struct fuse_file_info *fi)
{
	log_msg("\ntfs_release(path=\"%s\", fi=0x%08x)\n", path, fi);
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	log_fi (fi);
	log_msg("}\n");
	return 0;
}


int tfs_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
	log_msg("\ntfs_fsync(path=\"%s\", datasync=%d, fi=0x%08x)\n", path, datasync, fi);
	log_fi(fi);
	
	
#ifdef HAVE_FDATASYNC
	if (datasync)
		return log_syscall("fdatasync", fdatasync(fi->fh), 0);
	else
#endif	
		return log_syscall("fsync", fsync(fi->fh), 0);
}

#ifdef HAVE_SYS_XATTR_H

int tfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{

	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}

/** Get extended attributes */
int tfs_getxattr(const char *path, const char *name, char *value, size_t size)
{

	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("\t%s\n", value);
	log_msg("\t%s\n", name);
	//~ *value = 0;
	
	log_msg("}\n");
	return 0;
}

/** List extended attributes */
int tfs_listxattr(const char *path, char *list, size_t size)
{

	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}


int tfs_removexattr(const char *path, const char *name)
{

	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}
#endif


int tfs_opendir(const char *path, struct fuse_file_info *fi)
{

	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}


int tfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
		   struct fuse_file_info *fi)
{
	struct stat st;
	uint64_t i;

	
	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	if (!strcmp(path, "/")) {
		uint64_t fcnt = ftbl_getfcnt(tdata);
		
		log_msg("\treading /mnt (%llu files). why errors?\n", fcnt);
		
		for (i = 0; i < fcnt; ++i) {

			char rname[FNAME_MAXNAME + 1];

			ftbl_infstat(tdata, i, rname, NULL);
			
			log_msg("\t%s\n", rname);

			filler(buf, rname, &st, 0);
		}
		log_msg("\tfinished reading /mnt\n");
	}
	else {

		
		char dirname[FNAME_MAXNAME];
		const char *lpath = path;
		
		uint64_t *final_list;
		size_t flist_len = 0;
		while (lpath) {
			lpath = get_next_dir(lpath, dirname);
			if (lpath) {
				log_msg("dirname: %s\n", dirname);

				
				uint64_t dir_findex = ftbl_get_findex(tdata, dirname);
				log_msg("index %llu\n", dir_findex);
				uint64_t dir_start = ftbl_read_start(tdata, dir_findex);
				log_msg("start %llu\n", dir_start);
				
				size_t llist_len = 0;
				uint64_t *file_list;
				file_list = get_file_list(tdata, dir_start, &llist_len);
				
				if (!flist_len) {
					final_list = file_list;
					flist_len  = llist_len;
					log_msg("%llu\n", flist_len);
					log_msg("Creating first list\n");
				}
				else {
					log_msg("Merging\n");

					uint64_t *temp_list = (uint64_t *)malloc(sizeof(uint64_t) * flist_len);
					size_t tlen = 0;
					for (size_t fi = 0; fi < flist_len; ++fi) {
						int is_found = 0;
						log_msg("item = %llu\n", final_list[fi]);
						for (size_t li = 0; li < llist_len; ++li) {	
							if (file_list[li] == final_list[fi]) {
								is_found = 1;
								log_msg("found item = %llu\n", final_list[fi]);
								break;
							}
						}
						if (is_found) {
							temp_list[tlen] = final_list[fi];
							log_msg("added item: %llu\n", temp_list[tlen]);
							tlen++;
						}
					}
					
					memcpy(final_list, temp_list, sizeof(uint64_t) * tlen);
					flist_len = tlen;
						
					free(file_list);
					free(temp_list);
				}
			}
		}
		

		
		for (size_t i = 0; i < flist_len; ++i) {
			char name[FNAME_MAXNAME + 1];

			log_msg("processing: %llu ", final_list[i]);
			ftbl_infstat(tdata, final_list[i], name, NULL);
			log_msg("%s\n", name);
			filler(buf, name, &st, 0);
			log_msg("filled\n");
		}
		free(final_list);
	}
	//~ fi->mode = st.st_mode;
	
	log_msg("}\n");
	
	return 0;
}

int tfs_releasedir(const char *path, struct fuse_file_info *fi)
{

	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}


int tfs_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi)
{

	log_msg("%s {\n", __FUNCTION__);
	log_msg("\t%s\n", path);
	
	log_msg("}\n");
	return 0;
}


void *tfs_init(struct fuse_conn_info *conn) {
	tdata = TFS_DATA;
	log_msg("\ntfs_init()\n");
	
	log_conn(conn);
	log_fuse_context(fuse_get_context());
	
	tdata->devdesc = open(tdata->devpath, O_RDWR);
	
	if (tdata->devdesc) {

		uint64_t bsize;
		uint64_t bsize_detected;
		uint64_t bmapsz = 0;
		uint64_t fmapsz = 0;
		
		ioctl(tdata->devdesc, BLKGETSIZE, &bsize_detected);

		pread(tdata->devdesc, &bsize, sizeof(bsize), SINF_OFFSET);
		
		if (bsize_detected != bsize) {
			log_msg("Warning: size mismatch\n");
			log_msg("said: %llu\n", bsize);
			log_msg("real: %llu\n", bsize_detected);
		}
		

		bmapsz = bsize;
		

		tdata->fcnt = 0;
		tdata->fmap_offset = BASE_OFFSET + 8;
		tdata->btbl_offset = tdata->fmap_offset + fmapsz;
		tdata->ftbl_offset = tdata->btbl_offset + bmapsz;
		tdata->data_offset = tdata->ftbl_offset + 4096 * 1024; // TEMPORARY
		

	return TFS_DATA;
}


void tfs_destroy(void *userdata)
{
	log_msg("\ntfs_destroy(userdata=0x%08x)\n", userdata);
	close(TFS_DATA->devdesc);
}


int tfs_access(const char *path, int mask)
{
	int retstat = 0;
	char fpath[PATH_MAX];
   
	log_msg("\ntfs_access(path=\"%s\", mask=0%o)\n", path, mask);
	tfs_fullpath(fpath, path);
	
	retstat = access(fpath, mask);
	
	if (retstat < 0)
		retstat = log_error("tfs_access access");
	
	return retstat;
}

int tfs_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
{

	ftbl_setfsz(tdata, fi->fh, offset);
	return 0;
}

int tfs_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi)
{
	int retstat = 0;
	
	log_msg(
		"\ntfs_fgetattr(path=\"%s\", statbuf=0x%08x, fi=0x%08x)\n",
		path, statbuf, fi
	);
	log_fi(fi);

	if (!strcmp(path, "/"))
		return tfs_inner_getattr(path, statbuf, fi);
	
	retstat = 0;//fstat(fi->fh, statbuf);
	if (retstat < 0)
		retstat = log_error("tfs_fgetattr fstat");
	
	log_stat(statbuf);
	
	return retstat;
}

struct fuse_operations tfs_oper = {
  .getattr  = tfs_getattr,
  .readlink = tfs_readlink,
  .getdir   = NULL,
  .mknod    = tfs_mknod,
  .mkdir    = tfs_mkdir,
  .unlink   = tfs_unlink,
  .rmdir    = tfs_rmdir,
  .symlink  = tfs_symlink,
  .rename   = tfs_rename,
  .link     = tfs_link,
  .chmod    = tfs_chmod,
  .chown    = tfs_chown,
  .truncate = tfs_truncate,
  .utime    = tfs_utime,
  .open     = tfs_open,
  .read     = tfs_read,
  .write    = tfs_write,
  .statfs   = tfs_statfs,
  .flush    = tfs_flush,
  .release  = tfs_release,
  .fsync    = tfs_fsync,
  
#ifdef HAVE_SYS_XATTR_H
  .setxattr    = tfs_setxattr,
  .getxattr    = tfs_getxattr,
  .listxattr   = tfs_listxattr,
  .removexattr = tfs_removexattr,
#endif
  
  .opendir    = tfs_opendir,
  .readdir    = tfs_readdir,
  .releasedir = tfs_releasedir,
  .fsyncdir   = tfs_fsyncdir,
  .init       = tfs_init,
  .destroy    = tfs_destroy,
  .access     = tfs_access,
  .ftruncate  = tfs_ftruncate,
  .fgetattr   = tfs_fgetattr
};

void tfs_usage()
{
	fprintf(stderr, "usage:  mount.tfs [FUSE and mount options] rootDir mountPoint\n");
	abort();
}

int check_fs(char *path) {
	int devfd = open(path, O_RDONLY);
	int err   = 0;
		
	if (devfd >= 0) {
		uint64_t magic;

		pread(devfd, &magic, sizeof(magic), BASE_OFFSET);
		
		if (magic != MAGIC_NUMBER)
			err = -2;
			
		close (devfd);
	}
	else
		err = -1;

	return err;
}

int main(int argc, char *argv[])
{
	int fuse_stat;
	struct _tfs_state *tfs_data;

	if ((getuid() == 0) || (geteuid() == 0)) {
		fprintf(stderr, "Running TFS as root opens unnacceptable security holes\n");
		return 1;
	}

	fprintf(stderr, "Fuse library version %d.%d\n", FUSE_MAJOR_VERSION, FUSE_MINOR_VERSION);
	

	if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-'))
		tfs_usage();
		
	int err = check_fs(argv[argc - 2]);
	err = 0;
	if (!err) {
		tfs_data = malloc(sizeof(struct _tfs_state));
		if (tfs_data == NULL) {
			perror("main calloc");
			abort ();
		}

		tfs_data->devpath = realpath(argv[argc - 2], NULL);
		tfs_data->mntpath = realpath(argv[argc - 1], NULL);

		argv[argc - 2] = argv[argc - 1];
		argv[argc - 1] = NULL;
		argc--;
		
		tfs_data->logfile = log_open();
		

		fprintf(stderr, "about to call fuse_main\n");
		fuse_stat = fuse_main(argc, argv, &tfs_oper, tfs_data);
		fprintf(stderr, "fuse_main returned %d\n", fuse_stat);
	}
	else
		fprintf(stderr, "FS has an error (%d). Please format.\n", err);
	
	return fuse_stat;
}
