// gcc -Wall `pkg-config fuse --cflags` soal1.c -o soal1 `pkg-config fuse --libs`
#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
 
static const char *dirpath = "/home/fwildanf/Documents";
 
static int xmp_getattr(const char *path, struct stat *stbuf)
{
 	int res;
 	char fpath[1000];
 	sprintf(fpath,"%s%s",dirpath,path);
 	res = lstat(fpath, stbuf);
 	if (res == -1)
 	return -errno;
 	return 0;
}
 
static int xmp_readdir(const char *path, void *buf,
fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
 	DIR *dp;
 	struct dirent *de;
 	char fpath[1000];
 	sprintf(fpath,"%s%s",dirpath,path);
 	(void) offset;
 	(void) fi;
 	dp = opendir(fpath);
 	if (dp == NULL) return -errno;
 	while ((de = readdir(dp)) != NULL) {
 		struct stat st;
 		memset(&st, 0, sizeof(st));
 		st.st_ino = de->d_ino;
 		st.st_mode = de->d_type << 12;
 		if (filler(buf, de->d_name, &st, 0)) break;
	}
 	closedir(dp);
 	return 0;
}
 
int flags(const char *namafile)
{
 	int x = strlen(namafile);
 	char file[100];
 	strcpy(file,namafile+x-4);
 	if(strcmp(file,".pdf")==0 || strcmp(file,".doc")==0 || strcmp(file,".txt")==0) return 1;
 	else return 0;
}
 
 
static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	if(flags(fpath)){
	 	char command[100];
	 	sprintf(command,"zenity --error --text='Terjadi Kesalahan! File berisi konten berbahaya.\n\' --title=\"Warning!\"");
	 	system(command);
	 	return 1;
	}
	else{
		res = open(fpath, fi->flags);
		if(res == -1) return -errno;
	}
	close(res);
	return 0;
}
 
static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.open		= xmp_open,
};
 
int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
