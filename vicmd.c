#define risbuf(r) (risascii(r) &&(islower(r) ||isdigit(r) ) )  \

#define CIsDouble 1
#define CHasArg 2
#define CHasMotion 4
#define CIsMotion 8
#define CZeroCount 16 \

#define curb (&curwin->eb->b)  \

#define MaxInsert 512 \

#define risblank(r) (risascii(r) &&isblank(r) ) 
#define swap(p0,p1) {unsigned _tmp= p0;p0= p1,p1= _tmp;} \

#define risctrl(r) (r<0x1b)  \

#define Mtn(flags,f) {CIsMotion|flags,.motion= f}
#define Act(flags,f) {flags,.cmd= f}
/*1:*/
#line 19 "./vicmd.w"

/*2:*/
#line 33 "./vicmd.w"

#include <assert.h> 
#include <ctype.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include "unicode.h"
#include "buf.h"
#include "edit.h"
#include "gui.h"
#include "win.h"
#include "exec.h"
#include "cmd.h"

/*:2*/
#line 20 "./vicmd.w"

/*15:*/
#line 243 "./vicmd.w"

extern W*curwin;
void chwin(W*);

/*:15*/
#line 21 "./vicmd.w"

/*5:*/
#line 103 "./vicmd.w"

typedef struct{
unsigned short count;
unsigned char chr;
Rune arg;
}Cmd;

/*:5*//*22:*/
#line 359 "./vicmd.w"

typedef struct{
unsigned beg,end;
int linewise:1;
}Motion;

/*:22*//*80:*/
#line 1295 "./vicmd.w"

typedef int motion_t(int,Cmd,Motion*);
typedef int cmd_t(char,Cmd,Cmd);

/*:80*/
#line 22 "./vicmd.w"

/*24:*/
#line 387 "./vicmd.w"

static int m_hl(int,Cmd,Motion*);
static int m_jk(int,Cmd,Motion*);

/*:24*//*32:*/
#line 503 "./vicmd.w"

static int m_find(int,Cmd,Motion*);
static int m_repf(int,Cmd,Motion*);

/*:32*//*36:*/
#line 556 "./vicmd.w"

static int m_bol(int,Cmd,Motion*);
static int m_eol(int,Cmd,Motion*);
static int m_line(int,Cmd,Motion*);

/*:36*//*38:*/
#line 604 "./vicmd.w"

static int m_ewEW(int,Cmd,Motion*);
static int m_bB(int,Cmd,Motion*);


/*:38*//*48:*/
#line 776 "./vicmd.w"

static int m_par(int,Cmd,Motion*);
static int m_match(int,Cmd,Motion*);

/*:48*//*51:*/
#line 827 "./vicmd.w"

static int m_G(int,Cmd,Motion*);
static int m_HML(int,Cmd,Motion*);

/*:51*//*53:*/
#line 850 "./vicmd.w"

static int m_mark(int,Cmd,Motion*);

/*:53*//*55:*/
#line 880 "./vicmd.w"

static int m_nN(int,Cmd,Motion*);

/*:55*//*57:*/
#line 897 "./vicmd.w"

static int m_sel(int,Cmd,Motion*);

/*:57*//*81:*/
#line 1302 "./vicmd.w"

static cmd_t a_d,a_c,a_y,a_pP,a_m,a_ins,a_run,a_scroll,a_swtch,a_tag,a_write,a_exit;

/*:81*/
#line 23 "./vicmd.w"

/*3:*/
#line 50 "./vicmd.w"

enum{
Command= 'c',
Insert= 'i'
};
int mode= Command;


/*:3*//*14:*/
#line 231 "./vicmd.w"

static struct{
int flags;
/*82:*/
#line 1305 "./vicmd.w"

union{
motion_t*motion;
cmd_t*cmd;
};

/*:82*/
#line 234 "./vicmd.w"

}keys[128]= {/*83:*/
#line 1313 "./vicmd.w"

['h']= Mtn(0,m_hl),['l']= Mtn(0,m_hl),
['j']= Mtn(0,m_jk),['k']= Mtn(0,m_jk),
['t']= Mtn(CHasArg,m_find),['f']= Mtn(CHasArg,m_find),
['T']= Mtn(CHasArg,m_find),['F']= Mtn(CHasArg,m_find),
[',']= Mtn(0,m_repf),[';']= Mtn(0,m_repf),
['0']= Mtn(0,m_bol),['^']= Mtn(0,m_bol),
['$']= Mtn(0,m_eol),['_']= Mtn(0,m_line),
['w']= Mtn(0,m_ewEW),['W']= Mtn(0,m_ewEW),
['e']= Mtn(0,m_ewEW),['E']= Mtn(0,m_ewEW),
['b']= Mtn(0,m_bB),['B']= Mtn(0,m_bB),
['{']= Mtn(0,m_par),['}']= Mtn(0,m_par),
['%']= Mtn(0,m_match),['G']= Mtn(CZeroCount,m_G),
['H']= Mtn(0,m_HML),['L']= Mtn(0,m_HML),
['M']= Mtn(0,m_HML),
['n']= Mtn(0,m_nN),['N']= Mtn(0,m_nN),
['/']= Mtn(0,m_sel),
['\'']= Mtn(CHasArg,m_mark),['`']= Mtn(CHasArg,m_mark),
['d']= Act(CHasMotion,a_d),['x']= Act(0,a_d),
['c']= Act(CHasMotion,a_c),['y']= Act(CHasMotion,a_y),
['i']= Act(0,a_ins),['I']= Act(0,a_ins),
['a']= Act(0,a_ins),['A']= Act(0,a_ins),
['o']= Act(0,a_ins),['O']= Act(0,a_ins),
['p']= Act(0,a_pP),['P']= Act(0,a_pP),
['.']= Act(CZeroCount,0),['m']= Act(CHasArg,a_m),
[CTRL('D')]= Act(CZeroCount,a_scroll),
[CTRL('U')]= Act(CZeroCount,a_scroll),
[CTRL('E')]= Act(0,a_scroll),[CTRL('Y')]= Act(0,a_scroll),
[CTRL('T')]= Act(0,a_tag),[CTRL('I')]= Act(0,a_run),
[CTRL('L')]= Act(CHasArg,a_swtch),
[CTRL('W')]= Act(0,a_write),[CTRL('Q')]= Act(0,a_exit),

/*:83*/
#line 235 "./vicmd.w"
};


/*:14*//*16:*/
#line 254 "./vicmd.w"

static struct{
Rune buf[MaxInsert];
unsigned len;
int locked;
}lasti;
static unsigned short cnti;

/*:16*//*29:*/
#line 479 "./vicmd.w"

static struct{char locked;char chr;Rune arg;}lastf;

/*:29*//*60:*/
#line 940 "./vicmd.w"

static int ytip;
static YBuf ynum[9],yannon;

/*:60*/
#line 24 "./vicmd.w"

/*18:*/
#line 279 "./vicmd.w"

static void insert(Rune r)
{
if(!lasti.locked&&r!=GKEsc){
if(lasti.len<MaxInsert)
lasti.buf[lasti.len++]= r;
else
lasti.len= 0,lasti.locked= 1;
}

switch(r){
case GKEsc:/*19:*/
#line 301 "./vicmd.w"

lasti.locked= 1;
assert(cnti!=0);
while(--cnti)
for(unsigned u= 0;u<lasti.len;u++)
insert(lasti.buf[u]);
lasti.locked= 0;

if(buf_get(curb,curwin->cu-1)!='\n')
curwin->cu--;
eb_commit(curwin->eb),mode= Command;

/*:19*/
#line 290 "./vicmd.w"
;break;
case GKBackspace:/*20:*/
#line 313 "./vicmd.w"

if(curwin->cu> 0){
eb_del(curwin->eb,curwin->cu-1,curwin->cu);
curwin->cu--;
}

/*:20*/
#line 291 "./vicmd.w"
;break;
case'\n':/*21:*/
#line 320 "./vicmd.w"

eb_ins(curwin->eb,curwin->cu,'\n');
for(
unsigned p= buf_bol(curb,curwin->cu++);
r= buf_get(curb,p),risblank(r);
p++
)
eb_ins(curwin->eb,curwin->cu++,r);

/*:21*/
#line 292 "./vicmd.w"
;break;
default:eb_ins(curwin->eb,curwin->cu++,r);break;
}
}

/*:18*//*23:*/
#line 369 "./vicmd.w"

static unsigned blkspn(unsigned p)
{
Rune r;
do r= buf_get(curb,p++);while(risblank(r));
return p-1;
}

/*:23*//*25:*/
#line 399 "./vicmd.w"

static int m_hl(int ismotion,Cmd c,Motion*m)
{
int line,col;
buf_getlc(curb,m->beg,&line,&col);
if(c.chr=='h'){
if(col==0)return 1;
m->end= buf_setlc(curb,line,col-c.count);
if(ismotion)swap(m->beg,m->end);
}else{
if(buf_get(curb,m->beg)=='\n')return 1;
m->end= buf_setlc(curb,line,col+c.count);
if(!ismotion&&buf_get(curb,m->end)=='\n')return 1;
}
return 0;
}

/*:25*//*26:*/
#line 419 "./vicmd.w"

static int m_jk(int ismotion,Cmd c,Motion*m)
{
int line,col;
buf_getlc(curb,m->beg,&line,&col);
if(c.chr=='k'){
if(c.count> line)return 1;
m->end= buf_setlc(curb,line-c.count,col);
}else
m->end= buf_setlc(curb,line+c.count,col);
if(ismotion){
if(c.chr=='k')swap(m->beg,m->end);
/*27:*/
#line 439 "./vicmd.w"

{
m->linewise= 1;
m->beg= buf_bol(curb,m->beg);
m->end= buf_eol(curb,m->end)+1;
}

/*:27*/
#line 431 "./vicmd.w"
;
}
return 0;
}

/*:26*//*28:*/
#line 451 "./vicmd.w"

static int m_find(int ismotion,Cmd c,Motion*m)
{
int dp= islower(c.chr)?1:-1;
unsigned p= m->beg;
register Rune r;

/*30:*/
#line 482 "./vicmd.w"

if(!lastf.locked)lastf.chr= c.chr,lastf.arg= c.arg;
else lastf.locked= 0;

/*:30*/
#line 458 "./vicmd.w"
;
while(c.count--)
while((r= buf_get(curb,p+= dp))!=c.arg)

if(r=='\n')return 1;

m->end= tolower(c.chr)=='f'?p:p-dp;
if(ismotion){
if(dp==1)m->end++;
else swap(m->beg,m->end);
}
return 0;
}

/*:28*//*31:*/
#line 491 "./vicmd.w"

static int m_repf(int ismotion,Cmd c,Motion*m)
{
Cmd cf= {c.count,lastf.chr,lastf.arg};

if(lastf.chr==0)return 1;
if(c.chr==',')cf.chr^= 32;
lastf.locked= 1;
return m_find(ismotion,cf,m);
}


/*:31*//*33:*/
#line 513 "./vicmd.w"

static int m_bol(int ismotion,Cmd c,Motion*m)
{
m->end= buf_bol(curb,m->beg);
if(c.chr=='^')m->end= blkspn(m->end);
if(ismotion&&m->end<m->beg)swap(m->beg,m->end);
return ismotion&&m->end==m->beg;
}

/*:33*//*34:*/
#line 527 "./vicmd.w"

static int m_eol(int ismotion,Cmd c,Motion*m)
{
unsigned bol= buf_bol(curb,m->beg);
int l,x;


buf_getlc(curb,m->beg,&l,&x);
m->end= buf_eol(curb,buf_setlc(curb,l+c.count-1,0))-1;
if(ismotion||buf_get(curb,m->end)=='\n')m->end++;
if(ismotion&&c.count> 1&&blkspn(bol)>=m->beg)
m->linewise= 1,m->beg= bol,m->end++;

return ismotion&&c.count==1&&m->end==m->beg;
}

/*:34*//*35:*/
#line 547 "./vicmd.w"

static int m_line(int ismotion,Cmd c,Motion*m)
{
int r= m_jk(ismotion,(Cmd){c.count-1,'j',0},m);
if(ismotion||r)return r;
m->end= blkspn(buf_bol(curb,m->end));
return 0;
}

/*:35*//*37:*/
#line 567 "./vicmd.w"

static int risword(Rune r)
{
return(risascii(r)&&isalpha(r))
||(r>=0xc0&&r<0x100)

||(r>='0'&&r<='9')
||r=='_';
}

static int risbigword(Rune r)
{
return!risascii(r)||!isspace(r);
}

/*:37*//*39:*/
#line 613 "./vicmd.w"

static int m_ewEW(int ismotion,Cmd c,Motion*m)
{
int(*in)(Rune)= islower(c.chr)?risword:risbigword;
int dfa[][2]= {{1,0},{1,2}},ise= tolower(c.chr)=='e';
unsigned p= m->beg;
Rune r= 'x';

while(c.count--)
for(
int s= 0;
s!=2;
s= dfa[s][ise^in(r= buf_get(curb,ise+p++))]
){
if(p>=curb->limbo+1)break;
if(r=='\n'&&c.count==0&&ismotion&&!ise)break;
}
m->end= ismotion&&ise?p:p-1;
return 0;
}

/*:39*//*40:*/
#line 637 "./vicmd.w"

static int m_bB(int ismotion,Cmd c,Motion*m)
{
int(*in)(Rune)= c.chr=='b'?risword:risbigword;
int dfa[][2]= {{0,1},{2,1}};
unsigned p= m->beg;

while(c.count--)
for(
int s= 0;
s!=2&&p!=-1u;
s= dfa[s][in(buf_get(curb,--p))]
);
m->end= p+1;
if(ismotion)swap(m->beg,m->end);
return 0;
}

/*:40*//*41:*/
#line 666 "./vicmd.w"

static int m_par(int ismotion,Cmd c,Motion*m)
{
int l,x,dl= c.chr=='{'?-1:1;
enum{Blank,FormFeed,Text}ltyp;
int s,dfa[][3]= {
{0,3,3},
{2,2,3},
{2,9,3},
{9,9,3}
};
unsigned bol;

buf_getlc(curb,m->beg,&l,&x);
bol= buf_bol(curb,m->beg);
/*44:*/
#line 721 "./vicmd.w"

if(blkspn(bol)>=m->beg){
m->beg= bol;
m->linewise= 1;
}

/*:44*/
#line 681 "./vicmd.w"
;

while(c.count--)
for(
s= c.chr=='{';
l>=0&&(bol<curb->limbo||c.chr=='{');
){
/*42:*/
#line 697 "./vicmd.w"

switch(buf_get(curb,blkspn(bol))){
case'\n':ltyp= Blank;break;
case'\f':ltyp= FormFeed;break;
default:ltyp= Text;break;
}

/*:42*/
#line 688 "./vicmd.w"
;
/*43:*/
#line 709 "./vicmd.w"

if((s= dfa[s][ltyp])==9)break;
l+= dl,bol= buf_setlc(curb,l,0);

/*:43*/
#line 689 "./vicmd.w"
;
}

m->end= bol;
if(ismotion&&c.chr=='{')swap(m->beg,m->end);
return 0;
}

/*:41*//*45:*/
#line 731 "./vicmd.w"

static int m_match(int ismotion,Cmd c,Motion*m)
{
Rune match[]= {'<','{','(','[',']',')','}','>'};
int n,dp,N= sizeof match/sizeof match[0];
unsigned p= m->beg;
Rune beg,end,r;

/*46:*/
#line 756 "./vicmd.w"

for(;(r= beg= buf_get(curb,p))!='\n';p++)
for(n= 0;n<N;n++)
if(match[n]==r)goto found;
return 1;
found:dp= n>=N/2?-1:1,end= match[N-n-1];

/*:46*/
#line 739 "./vicmd.w"
;
for(
n= 0;
(n+= (r==beg)-(r==end))!=0;
r= buf_get(curb,p+= dp)
)
if(p==-1u||p>=curb->limbo)return 1;
m->end= p;
if(ismotion)
/*47:*/
#line 767 "./vicmd.w"

{
if(dp==-1)swap(m->beg,m->end);
m->end++;
if(blkspn(buf_bol(curb,m->beg))>=m->beg
&&blkspn(m->end)==buf_eol(curb,m->end))
/*27:*/
#line 439 "./vicmd.w"

{
m->linewise= 1;
m->beg= buf_bol(curb,m->beg);
m->end= buf_eol(curb,m->end)+1;
}

/*:27*/
#line 773 "./vicmd.w"
;
}

/*:47*/
#line 748 "./vicmd.w"
;
return 0;
}

/*:45*//*49:*/
#line 784 "./vicmd.w"

static int m_G(int ismotion,Cmd c,Motion*m)
{
m->end= c.count?buf_setlc(curb,c.count-1,0):curb->limbo;
if(!ismotion)
m->end= blkspn(m->end);
else{
if(m->end<m->beg)swap(m->beg,m->end);
/*27:*/
#line 439 "./vicmd.w"

{
m->linewise= 1;
m->beg= buf_bol(curb,m->beg);
m->end= buf_eol(curb,m->end)+1;
}

/*:27*/
#line 792 "./vicmd.w"
;
}
return 0;
}

/*:49*//*50:*/
#line 803 "./vicmd.w"

static int m_HML(int ismotion,Cmd c,Motion*m)
{
if((c.chr=='H'||c.chr=='L')
&&c.count> curwin->nl)
return 1;
switch(c.chr){
case'H':
m->end= curwin->l[c.count-1];
break;
case'L':
m->end= curwin->l[curwin->nl-c.count];
break;
case'M':
m->end= curwin->l[curwin->nl/2];
break;
}
if(ismotion){
if(m->end<m->beg)swap(m->beg,m->end);
/*27:*/
#line 439 "./vicmd.w"

{
m->linewise= 1;
m->beg= buf_bol(curb,m->beg);
m->end= buf_eol(curb,m->end)+1;
}

/*:27*/
#line 822 "./vicmd.w"
;
}
return 0;
}

/*:50*//*52:*/
#line 836 "./vicmd.w"

static int m_mark(int ismotion,Cmd c,Motion*m)
{
if((m->end= eb_getmark(curwin->eb,c.arg))==-1u)
return 1;
if(ismotion){
if(m->end<m->beg)swap(m->beg,m->end);
if(c.chr=='\'')/*27:*/
#line 439 "./vicmd.w"

{
m->linewise= 1;
m->beg= buf_bol(curb,m->beg);
m->end= buf_eol(curb,m->end)+1;
}

/*:27*/
#line 843 "./vicmd.w"
;
}
else if(c.chr=='\'')
m->end= blkspn(buf_bol(curb,m->end));
return 0;
}

/*:52*//*54:*/
#line 860 "./vicmd.w"

static int m_nN(int ismotion,Cmd c,Motion*m)
{
YBuf b= {0,0,0,0},*pb= &yannon;
int err= 0;
unsigned s0,s1;

s0= eb_getmark(curwin->eb,SelBeg);
s1= eb_getmark(curwin->eb,SelEnd);
if(s0<s1&&s0!=-1u&&s1!=-1u)
eb_yank(curwin->eb,s0,s1,pb= &b);
while(c.count--)
err|= ex_look(curwin,pb->r,pb->nr,c.chr=='N');
free(b.r);
m->end= curwin->cu;
if(ismotion)
/*27:*/
#line 439 "./vicmd.w"

{
m->linewise= 1;
m->beg= buf_bol(curb,m->beg);
m->end= buf_eol(curb,m->end)+1;
}

/*:27*/
#line 876 "./vicmd.w"
;
return err;
}

/*:54*//*56:*/
#line 888 "./vicmd.w"

static int m_sel(int ismotion,Cmd c,Motion*m)
{
if(!ismotion||c.count!=1)return 1;
m->beg= eb_getmark(curwin->eb,SelBeg);
m->end= eb_getmark(curwin->eb,SelEnd);
return m->beg>=m->end||m->end==-1u;
}

/*:56*//*61:*/
#line 944 "./vicmd.w"

static void yankspan(Motion*m,YBuf*y)
{
eb_yank(curwin->eb,m->beg,m->end,y);
y->linemode= m->linewise;
}

static int yank(Motion*m,char buf,unsigned count,Cmd mc)
{
mc.count*= count;

*m= (Motion){curwin->cu,0,0};

assert(keys[mc.chr].flags&CIsMotion);
if(keys[mc.chr].motion(1,mc,m))
return 1;

if(m->linewise)
yankspan(m,&ynum[ytip= (ytip+8)%9]);
yankspan(m,&yannon);

return 0;
}

/*:61*//*62:*/
#line 972 "./vicmd.w"

static int a_y(char buf,Cmd c,Cmd mc)
{
Motion m= {0,0,0};
int r= yank(&m,buf,c.count,mc);

if(r==0){
eb_setmark(curwin->eb,SelBeg,m.beg);
eb_setmark(curwin->eb,SelEnd,m.end);
}
return r;
}

/*:62*//*63:*/
#line 989 "./vicmd.w"

static int a_pP(char buf,Cmd c,Cmd mc)
{
YBuf*y= &yannon;

(void)mc;
if(buf>='1'&&buf<='9')
y= &ynum[(ytip+buf-'1')%9];
else if(buf!=0)return 1;

/*64:*/
#line 1008 "./vicmd.w"

if(y->linemode&&c.chr=='P')
curwin->cu= buf_bol(curb,curwin->cu);
else if(y->linemode&&c.chr=='p')
curwin->cu= buf_eol(curb,curwin->cu)+1;
else if(c.chr=='p'&&buf_get(curb,curwin->cu)!='\n')
curwin->cu++;

/*:64*/
#line 999 "./vicmd.w"
;

while(c.count--)
for(unsigned p= 0;p<y->nr;p++)
eb_ins(curwin->eb,curwin->cu+p,y->r[p]);
eb_commit(curwin->eb);
return 0;
}

/*:63*//*65:*/
#line 1020 "./vicmd.w"

static int a_d(char buf,Cmd c,Cmd mc)
{
Motion m;

if(c.chr=='x')mc= (Cmd){1,'l',0};
if(yank(&m,buf,c.count,mc))return 1;
eb_del(curwin->eb,curwin->cu= m.beg,m.end);
eb_commit(curwin->eb);
return 0;
}

static int a_c(char buf,Cmd c,Cmd mc)
{
Motion m;

if(yank(&m,buf,c.count,mc))return 1;
if(m.linewise){
m.beg= blkspn(m.beg),m.end--;
assert(buf_get(curb,m.end)=='\n');
}
eb_del(curwin->eb,curwin->cu= m.beg,m.end);
/*17:*/
#line 265 "./vicmd.w"

if(!lasti.locked)
lasti.len= 0;
cnti= 1;
mode= Insert;

/*:17*/
#line 1042 "./vicmd.w"
;
return 0;
}

/*:65*//*66:*/
#line 1050 "./vicmd.w"

static int a_m(char buf,Cmd c,Cmd mc)
{
(void)buf;(void)mc;
eb_setmark(curwin->eb,c.arg,curwin->cu);
return 0;
}

/*:66*//*67:*/
#line 1061 "./vicmd.w"

static int a_write(char buf,Cmd c,Cmd mc)
{
(void)buf;(void)c;(void)mc;
return ex_put(curwin->eb,0);
}

/*:67*//*68:*/
#line 1068 "./vicmd.w"

static int a_exit(char buf,Cmd c,Cmd mc)
{
extern int exiting;
(void)buf;(void)c;(void)mc;
return(exiting= 1);
}

/*:68*//*69:*/
#line 1082 "./vicmd.w"

static int a_scroll(char buf,Cmd c,Cmd mc)
{
extern int scrolling;
static int lastud= 0;
int cnt;

(void)buf;(void)mc;
scrolling= 1;
switch(c.chr){
case CTRL('E'):
cnt= +c.count;
break;
case CTRL('Y'):
cnt= -c.count;
break;
case CTRL('U'):
case CTRL('D'):
if(c.count)lastud= c.count;
cnt= curwin->nl/3;
if(lastud)cnt= lastud;
if(c.chr==CTRL('U'))cnt= -cnt;
break;
}
win_scroll(curwin,cnt);
return 0;
}

/*:69*//*70:*/
#line 1117 "./vicmd.w"

static int a_tag(char buf,Cmd c,Cmd mc)
{
(void)buf;(void)c;(void)mc;
return chwin(win_tag_toggle(curwin)),0;
}

/*:70*//*71:*/
#line 1129 "./vicmd.w"

static int a_swtch(char buf,Cmd c,Cmd mc)
{
(void)buf;(void)mc;
switch(c.arg){
default:
return 1;
case'h':
case'j':
case'k':
case'l':
return chwin(win_edge(curwin,c.arg)),0;
}
}

/*:71*//*72:*/
#line 1149 "./vicmd.w"

static int a_run(char buf,Cmd c,Cmd mc)
{
(void)buf;(void)c;(void)mc;
return ex_run(curwin,curwin->cu);
}

/*:72*//*73:*/
#line 1165 "./vicmd.w"

static int a_ins(char buf,Cmd c,Cmd mc)
{
unsigned cu;

(void)buf;(void)mc;
if(c.chr=='a'&&curwin->cu!=buf_eol(curb,curwin->cu))
curwin->cu++;
if(c.chr=='A'||c.chr=='o')
curwin->cu= buf_eol(curb,curwin->cu);
if(c.chr=='I'||c.chr=='O')
cu= curwin->cu= blkspn(buf_bol(curb,curwin->cu));
/*17:*/
#line 265 "./vicmd.w"

if(!lasti.locked)
lasti.len= 0;
cnti= 1;
mode= Insert;

/*:17*/
#line 1177 "./vicmd.w"
;
cnti= c.count;
if(c.chr=='o')insert('\n');
if(c.chr=='O')insert('\n'),curwin->cu= cu;
return 0;
}

/*:73*//*74:*/
#line 1193 "./vicmd.w"

static void docmd(char buf,Cmd c,Cmd m)
{
static char lastbuf;
static Cmd lastc,lastm;
static int redo;

if(keys[c.chr].flags&CIsMotion){
Motion m= {curwin->cu,0,0};
if(keys[c.chr].motion(0,c,&m)==0)
curwin->cu= m.end;
return;
}

/*76:*/
#line 1236 "./vicmd.w"

if(c.chr=='.'){
Cmd cpyc= lastc,cpym= lastm;

if(lastc.chr==0)return;
assert(lastc.chr!='.');

if(lastc.chr=='u')
redo= !redo;
else
assert(redo==0);

if(c.count){
lastm.count= 1;
lastc.count= c.count;
}

lastf.locked= lasti.locked= 1;
docmd(lastbuf,lastc,lastm);
if(mode==Insert)
/*77:*/
#line 1267 "./vicmd.w"

{unsigned p= 0;

if(lastc.chr=='o'||lastc.chr=='O')
p= 1;
while(p<lasti.len)
insert(lasti.buf[p++]);
insert(GKEsc);
}

/*:77*/
#line 1256 "./vicmd.w"
;
lastf.locked= lasti.locked= 0;

lastc= cpyc,lastm= cpym;
return;
}

/*:76*/
#line 1207 "./vicmd.w"
;
/*78:*/
#line 1281 "./vicmd.w"

if(c.chr=='u'){
redo= !redo;
eb_undo(curwin->eb,redo,&curwin->cu);
lastc= c;
return;
}else
redo= 0;


/*:78*/
#line 1208 "./vicmd.w"
;

if(keys[c.chr].cmd!=0){
if(keys[c.chr].cmd(buf,c,m)||risctrl(c.chr))
return;
lastbuf= buf,lastc= c,lastm= m;
}
}

/*:74*/
#line 25 "./vicmd.w"

/*4:*/
#line 65 "./vicmd.w"

void
cmd_parse(Rune r)
{
/*6:*/
#line 110 "./vicmd.w"

static char buf;
static Cmd c,m,*pcmd= &c;
static enum{
BufferDQuote,
BufferName,
CmdChar,
CmdDouble,
CmdArg
}state= BufferDQuote;

/*:6*/
#line 69 "./vicmd.w"
;
switch(mode){
case Insert:insert(r);break;
case Command:/*7:*/
#line 126 "./vicmd.w"

if(r==GKEsc)/*13:*/
#line 212 "./vicmd.w"

{m.count= c.count= 0;
buf= 0;
pcmd= &c;
state= BufferDQuote;
}

/*:13*/
#line 127 "./vicmd.w"

else
switch(state){
case BufferDQuote:/*8:*/
#line 143 "./vicmd.w"

if(r=='"')
state= BufferName;
else{
state= CmdChar;
cmd_parse(r);
}

/*:8*/
#line 130 "./vicmd.w"
;break;
case BufferName:/*9:*/
#line 157 "./vicmd.w"

if(!risbuf(r))goto err;
buf= r;
state= CmdChar;

/*:9*/
#line 131 "./vicmd.w"
;break;
case CmdChar:/*12:*/
#line 180 "./vicmd.w"

if(!risascii(r))goto err;

if(isdigit(r)&&(r!='0'||pcmd->count)){
pcmd->count= 10*pcmd->count+(r-'0');
}else{
if(pcmd->count==0)
pcmd->count= !(keys[r].flags&CZeroCount);
pcmd->chr= r;
if(keys[pcmd->chr].flags&CIsDouble){
state= CmdDouble;break;
}
gotdbl:
if(keys[pcmd->chr].flags&CHasArg){
state= CmdArg;break;
}
gotarg:
if(pcmd==&m&&!(keys[pcmd->chr].flags&CIsMotion))
goto err;
if(keys[pcmd->chr].flags&CHasMotion){
assert(pcmd==&c);
pcmd= &m;break;
}
docmd(buf,c,m);
/*13:*/
#line 212 "./vicmd.w"

{m.count= c.count= 0;
buf= 0;
pcmd= &c;
state= BufferDQuote;
}

/*:13*/
#line 204 "./vicmd.w"
;
}

/*:12*/
#line 132 "./vicmd.w"
;break;
case CmdDouble:/*10:*/
#line 166 "./vicmd.w"

if(r!=pcmd->chr)
goto err;
goto gotdbl;

/*:10*/
#line 133 "./vicmd.w"
;break;
case CmdArg:/*11:*/
#line 171 "./vicmd.w"

pcmd->arg= r;
goto gotarg;

/*:11*/
#line 134 "./vicmd.w"
;break;
default:abort();
}

/*:7*/
#line 72 "./vicmd.w"
;break;
}
return;
err:puts("! invalid command");

/*13:*/
#line 212 "./vicmd.w"

{m.count= c.count= 0;
buf= 0;
pcmd= &c;
state= BufferDQuote;
}

/*:13*/
#line 77 "./vicmd.w"
;
}


/*:4*/
#line 26 "./vicmd.w"


/*:1*/
