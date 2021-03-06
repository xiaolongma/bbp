#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/fault.h>
#include <sys/file.h>
#include <sys/procfs.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>

void *check_malloc(int);
FILE *fopfile(char*, char*);

main(int ac,char **av)
{
FILE *fpr, *fopfile();
float *per, *sa1, *sa2, *res1, *res2, *avgr, v[8];
int i, np2, np;

char statinfo[512], string[512];
char eq[16], mag[16], stat[16], lon[16], lat[16];
char seqno[16], vs30[16], cd[16], xc[16], yc[16];

int sa_field = 6;
int print_header = 0;

char datafile1[128], datafile2[128];
char simfile1[128], simfile2[128];
char comp1[128], comp2[128];

simfile1[0] = '\0';
simfile2[0] = '\0';

setpar(ac,av);
mstpar("datafile1","s",datafile1);
mstpar("datafile2","s",datafile2);
getpar("simfile1","s",simfile1);
getpar("simfile2","s",simfile2);
mstpar("comp1","s",comp1);
mstpar("comp2","s",comp2);
mstpar("eqname","s",eq);
mstpar("mag","s",mag);
mstpar("stat","s",stat);
mstpar("lon","s",lon);
mstpar("lat","s",lat);
mstpar("seqno","s",seqno);
mstpar("vs30","s",vs30);
mstpar("cd","s",cd);
mstpar("xcos","s",xc);
mstpar("ycos","s",yc);
getpar("print_header","d",&print_header);
getpar("sa_field","d",&sa_field);
endpar();

sprintf(statinfo,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",eq,mag,stat,lon,lat,seqno,vs30,cd,xc,yc);

fpr = fopfile(datafile1,"r");

fgets(string,512,fpr);
fgets(string,512,fpr);
fgets(string,512,fpr);
fgets(string,512,fpr);

sscanf(string,"%d",&np);

per = (float *)check_malloc(np*sizeof(float));
sa1 = (float *)check_malloc(np*sizeof(float));
sa2 = (float *)check_malloc(np*sizeof(float));
res1 = (float *)check_malloc(np*sizeof(float));
res2 = (float *)check_malloc(np*sizeof(float));
avgr = (float *)check_malloc(np*sizeof(float));

for(i=0;i<np;i++)
   {
   fgets(string,512,fpr);
   sscanf(string,"%f %f %f %f %f %f %f %f",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7]);
   per[i] = v[1];
   sa1[i] = v[sa_field-1];
   }

fclose(fpr);

fpr = fopfile(datafile2,"r");

fgets(string,512,fpr);
fgets(string,512,fpr);
fgets(string,512,fpr);
fgets(string,512,fpr);

sscanf(string,"%d",&np2);

if(np2 != np)
   {
   fprintf(stderr,"No. periods not equal, exiting...\n");
   exit(-1);
   }

for(i=0;i<np;i++)
   {
   fgets(string,512,fpr);
   sscanf(string,"%f %f %f %f %f %f %f %f",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7]);
   sa2[i] = v[sa_field-1];
   }

fclose(fpr);

if(simfile1[0] == '\0' || simfile2[0] == '\0')
   {
   for(i=0;i<np;i++)
      {
      res1[i] = sa1[i];
      res2[i] = sa2[i];
      avgr[i] = sqrt(res1[i]*res2[i]);
      }
   }
else
   {
   fpr = fopfile(simfile1,"r");

   fgets(string,512,fpr);
   fgets(string,512,fpr);
   fgets(string,512,fpr);
   fgets(string,512,fpr);

   sscanf(string,"%d",&np2);

   if(np2 != np)
      {
      fprintf(stderr,"No. periods not equal, exiting...\n");
      exit(-1);
      }

   for(i=0;i<np;i++)
      {
      fgets(string,512,fpr);
      sscanf(string,"%f %f %f %f %f %f %f %f",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7]);

      res1[i] = -99;
      if(v[sa_field-1] != 0.0)
         res1[i] = log(sa1[i]/v[sa_field-1]);
      }

   fclose(fpr);

   fpr = fopfile(simfile2,"r");

   fgets(string,512,fpr);
   fgets(string,512,fpr);
   fgets(string,512,fpr);
   fgets(string,512,fpr);

   sscanf(string,"%d",&np2);

   if(np2 != np)
      {
      fprintf(stderr,"No. periods not equal, exiting...\n");
      exit(-1);
      }

   for(i=0;i<np;i++)
      {
      fgets(string,512,fpr);
      sscanf(string,"%f %f %f %f %f %f %f %f",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7]);

      res2[i] = -99;
      if(v[sa_field-1] != 0.0)
         res2[i] = log(sa2[i]/v[sa_field-1]);

      avgr[i] = 0.5*(res1[i]+res2[i]);
      }

   fclose(fpr);
   }

if(print_header)
   {
   fprintf(stdout,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s","EQ",
                                                               "Mag",
                                                               "stat",
                                                               "lon",
                                                               "lat",
                                                               "stat_seq_no",
                                                               "Vs30",
                                                               "close_dist",
                                                               "Xcos",
                                                               "Ycos",
                                                               "comp");
   for(i=0;i<np;i++)
      fprintf(stdout,"\t%13.5e",per[i]);

   fprintf(stdout,"\n");
   }

fprintf(stdout,"%s",statinfo);
fprintf(stdout,"\t%s",comp1);
for(i=0;i<np;i++)
   fprintf(stdout,"\t%13.5e",res1[i]);

fprintf(stdout,"\n");

fprintf(stdout,"%s",statinfo);
fprintf(stdout,"\t%s",comp2);
for(i=0;i<np;i++)
   fprintf(stdout,"\t%13.5e",res2[i]);

fprintf(stdout,"\n");

fprintf(stdout,"%s",statinfo);
fprintf(stdout,"\t%s","avgh");
for(i=0;i<np;i++)
   fprintf(stdout,"\t%13.5e",avgr[i]);

fprintf(stdout,"\n");
}

void *check_malloc(int len)
{
char *ptr;

ptr = (char *) malloc (len);

if(ptr == NULL)
   {
   fprintf(stderr,"*****  memory allocation error\n");
   exit(-1);
   }

return(ptr);
}

FILE *fopfile(char *name,char *mode)
{
FILE *fp;

if((fp = fopen(name,mode)) == NULL)
   {
   fprintf(stderr,"CAN'T FOPEN FILE = %s, MODE = %s\n", name, mode);
   exit(-1);
   }
return(fp);
}
