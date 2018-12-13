#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include <stdarg.h>
#include<iostream>
#include"compiler.h"
#include<fstream>
#include<sstream>

using namespace std;
ofstream out;
ifstream in;
stringstream ss;

int linenum = 1;					//源程序行号
bool hasprintf = false;			//是否有printf;
bool hasscanf = false;			//是否有scanf
int  call_rule = 0;				//调用规则,0为C,1为stdcall

string name[MAXWORD];	//存标识符
/*1*/
//string name[MAXWORD];
//char asmpr[MAXOP][150];		//存汇编指令

int NumOfC = 0;					//符号表内元素的个数
Character Char[MAXC];			//符号表
opr  opra[MAXOP];				//存四元式
//int	asmorder = 0;					//汇编指令下标变量
int oprnum = 0;					//四元式数
int order = 0;					//四元式变量下标
int lev = 0;						//层数

char re[10];					//用来传递临时变量的综合属性
string tempname;				//用来存放临时变量名
int errornum = 0, error[MAXERROR], errorline[MAXERROR];//错误数,错误信息代码和错误所在行
string trans;//添加开头的标识中转

//char tempstringname[10];		//存放临时字符串名字
int stringorder = 0;				//存放字符串标号

int lableorder = 0;				//标号下标变量
int adr = 0;						//每个函数内变量偏移地址
string shelltoken;			//最后解释程序的token

int lastsym = 0;//上一个字符
int type = 0;
int snum = 0;
int start;
int breakopr, Isbreak = 0;
int typeofexp1 = 11, typeofexp2 = 12;
int oprnumcase = 0;
int Isret = 0;

int globalvar_end = 0;//全局变量结束的位置


//错误处理
void error_msg(int i)
{
	//fprintf(FOUT, "line(%d)\terror%d:\t\t%s\n", errorline[errornum], i, err_msg[i]);
	if (out.is_open())
	{
		out << "line" << errorline[errornum] << "\terrord" << i << ":\t\t" << err_msg[i] << "\n";
	}
}


//字符获取和处理
int getsym()
{

	char ch;//读入的字符				
	int i, j, m;
	//读到第一个非空字符
		in.get(ch);
		//ch = fgetc(FIN);//读入一个字符
	while (isspace(ch))//处理空格
	{
		out << ch;
		//putc(ch, FOUT);//直接输出
		if (ch == '\n')
		{
			linenum++;//行号加1
			printf("%d:\t", linenum);//输出行号
		}
		in.get(ch);
		//ch = fgetc(FIN);
	}

	i = 0;//
	Word[i] = ch;//连接进去
	i++;

	//判断这个字符
	//如果是字母
	if (isalpha(ch) || ch == '_')
	{
		out << ch;
		//putc(ch, FOUT);//输出
		in.get(ch);
		//ch = fgetc(FIN);
		while (isalpha(ch) || isdigit(ch) || ch == '_')//如果是字符数字
		{
			Word[i] = ch;//连接
			i++;
			if (i == MAXWORD)//标识符长度判断
			{
				errorline[errornum] = linenum;//错误处理
				error[errornum++] = 1;
				i--;
				Word[i] = '\0';//结束
				//跳过过长的部分
				out << ch;
				in.get(ch);
				//putc(ch, FOUT);
				//ch = fgetc(FIN);
				while (isalpha(ch) || isdigit(ch) || ch == '_')
				{
					out << ch;
					in.get(ch);
					//putc(ch, FOUT);
					//ch = fgetc(FIN);
				}
				in.seekg(1, ios::cur);
				//ungetc(ch, FIN);//回退一个
				lastsym = 1;
				return 1;//是标识符
			}
			out << ch;
			in.get(ch);
			//putc(ch, FOUT);
			//ch = fgetc(FIN);
		}
		//回退一个,结束
		in.seekg(1, ios::cur);
		//ungetc(ch, FIN);
		//Word[i] = '\0';

		for (m = 0; m < kk; m++)//遍历找是否为keyword
		{
			if (Keyword[m]==Word)	//读取的字符串符合对key word的判断
			{
				lastsym = m + 10;
				return m + 10;//返回相应的keyword的对应数字
			}
		}
		if (m == kk) {
			//不区分大小写,变成大写
			for (i = 0;Word.size() ; i++)//Word[i] != '\0'
			{
				Word[i] = toupper(Word[i]);
			}
			trans = "_";
			//strcpy_s(trans, "_");
			trans += Word;
			//strcat_s(trans, Word);
			//strcpy_s(Word, trans);
			Word = trans;
			lastsym = 1;
			return 1;//不是key word,是标识符
		}
	}

	//数值处理
	if (isdigit(ch))
	{
		//putc(ch, FOUT);//输出
		out << ch;
		while (in.get(ch))
		{
			if (isdigit(ch))
				break;
			out << ch;
			//putc(ch, FOUT);
			Word[i++] = ch;
			if (i == MAXWORD)
			{
				errorline[errornum] = linenum;
				error[errornum++] = 1;
				Word[--i] = '\0';
				//跳过过长部分
				in.get(ch);
				//ch = fgetc(FIN);
				while (isdigit(ch))
				{
					out << ch;
					in.get(ch);
					//putc(ch, FOUT);
					//ch = fgetc(FIN);
				}
				ss >> Word;
				ss << num;
				//num = ;//将字符串转化为对应的数字
				//ungetc(ch, FIN);
				in.seekg(1, ios::cur);
				lastsym = 2;
				return 2;//是数字
			}

		}
		in.seekg(1, ios::cur);
		//ungetc(ch, FIN);
		Word[i] = '\0';
		ss >> Word;
		ss << num;
		//num = atoi(Word);//将字符串转化为对应的数字
		lastsym = 2;
		return 2;//是数字
	}


	//分界符
	i = 34;
	out << ch;
	//putc(ch, FOUT);
	switch (ch) {

		//case'\'':
		//			ch=fgetc(FIN);
		//			if(ch=='+'||ch=='-'||ch=='*'||ch=='\''||isalpha(ch)) 
		//				//对应于printf语句中的各种参数
		//			{
		//				cischar=ch;
		//				putc(ch,FOUT);
		//				if ((ch=fgetc(FIN))=='\'')
		//				{
		//					putc(ch,FOUT);
		//					return 4;//字符
		//				}
		//				else 
		//				{
		//					putc(ch,FOUT);
		//					errorline[errornum]=linenum;
		//					error[errornum++]=3;
		//					return -1;
		//				}
		//			}

	case'"'://处理“”
		j = 0;
		while ((ch = fgetc(FIN)) != '"')//不是非空串,拼合
		{
			//putc(ch, FOUT);
			out << ch;
			sisstring[j++] = ch;//拼合

			if (j == MAXLENGTH)
			{
				printf("warning!字符串过长\n");
				sisstring[--j] = '\0';
				//结束
				out << ch;
				//putc(ch,FOUT);
				//ch = fgetc(FIN);
				in.get(ch);
				//跳过
				while (ch != '"')
				{
					out << ch;
					//putc(ch, FOUT);
					j++;
					if (ch == EOF || j > 100)	//字符串过长或缺少反引号
					{
						errorline[errornum] = linenum;
						error[errornum++] = 5;
						return -1;
					}
				}
				getstringname();//获得标号,写入标号
				//strcpy_s(storestring[stringorder++].sisstring, sisstring);//存入字符串
				storestring[stringorder++].sisstring = sisstring;
				lastsym = 5;
				return 5;//返回字符串
			}
		}
		out << ch;
		//putc(ch, FOUT);//输出两边界符
		sisstring[j] = '\0';//字符串结尾
		getstringname();//获得标号,写入标号
		storestring[stringorder++].sisstring = sisstring;
		//strcpy_s(storestring[stringorder++].sisstring, sisstring);//存入字符串	
		lastsym = 5;
		return 5;//返回空字符串
	case'}':
		Word[1] = '\0';
		lastsym = 35;
		return 35;
	case'(':
		i++;
		Word[1] = '\0';
		lastsym = 37;
		return 37;
	case')':
		i++;
		Word[1] = '\0';
		lastsym = 36;
		return 36;
	case';':
		i++;
		Word[1] = '\0';
		lastsym = 38;
		return 38;
	case',':
		i++;
		Word[1] = '\0';
		lastsym = 39;
		return 39;
	case '{':
		i++;
		Word[1] = '\0';
		lastsym = 40;
		return 40;
	case'/':
		if ((ch = in.get()) == '/')//若为单行注释标志
		{
			out << ch;
			//putc(ch, FOUT);
			while ((ch = in.get()) != '\n')
				//putc(ch, FOUT);//注释内容不处理，直接输出
				cout << ch;
			out << ch;
			//putc(ch, FOUT);
			linenum++;
			printf("%d:\t", linenum);
			return (getsym());//递归调用，处理下一行代码,不必用goto
		}
		//ungetc(ch, FIN);
		in.seekg(1, ios::cur);
		Word[1] = '\0';
		lastsym = 34;
		return 34; //仅为/
	case'*':
		Word[1] = '\0';
		lastsym = 33;
		return 33;
	case '-':
	case'+':
		if (lastsym == 37 || lastsym == 45 || lastsym == 46 || lastsym == 47 || lastsym == 48 || lastsym == 49 || lastsym == 50 || lastsym == 51)//识别为正数
		{
			int temp;
			j = 0;
			Word[j++] = ch;
			temp = ch;
			//ch = fgetc(FIN);
			in.get(ch);
			if (isdigit(ch))
			{
				Word[j++] = ch;
				out << ch;
				//putc(ch, FOUT);
				while (isdigit(ch = in.get()))
				{
					Word[j++] = ch;
					out << ch;
					//putc(ch, FOUT);//拼合数字
					if (i == MAXWORD)
					{
						errorline[errornum] = linenum;
						error[errornum++] = 1;
						Word[--j] = '\0';
						//ch = fgetc(FIN);
						in.get(ch);
						while (isdigit(ch))
						{
							out << ch;
							//putc(ch, FOUT);
						}
						ss >> Word;
						ss << num;
						//num = atoi(Word);
						//ungetc(ch, FIN);
						in.seekg(1, ios::cur);
						lastsym = 2;
						return 2;//带符号的数字
					}
				}
				//ungetc(ch, FIN);
				in.seekg(1, ios::cur);
				Word[j] = '\0';
				//num = atoi(Word);
				ss >> Word;
				ss << num;
				lastsym = 2;
				return 2;//带符号的数字
			}
			else
			{
				//ungetc(ch, FIN);
				in.seekg(1, ios::cur);
				if (temp == '-')
				{
					lastsym = 32;
					return 32;
				}
				if (temp == '+')
				{
					lastsym = 31;
					return 31;
				}
			}

		}
		else			//是运算符
		{
			if (ch == '-')
			{
				lastsym = 32;
				return 32;
			}
			if (ch == '+')
			{
				lastsym = 31;
				return 31;
			}
		}
	case'<':
		if ((ch =in.get()) == '=')
		{
			out << ch;
			//putc(ch, FOUT);
			Word[2] = '\0';
			lastsym = 50;
			return 50;//	<=
		}
		else {
			out << ch;
			//ungetc(ch, FIN);
			Word[1] = '\0';
			lastsym = 46;
			return 46;//	<
		}
	case'>':
		if ((ch = fgetc(FIN)) == '=')
		{
			out << ch;
			//putc(ch, FOUT);
			Word[2] = '\0';
			lastsym = 49;
			return 49;//	>=
		}
		else
		{
			out << ch;
			//ungetc(ch, FIN);
			Word[1] = '\0';
			lastsym = 45;
			return 45;//	>
		}
	case'=':
		if ((ch = in.get()) == '=')
		{
			out << ch;
			//putc(ch, FOUT);
			Word[2] = '\0';
			lastsym = 47;
			return 47;//	==
		}
		else
		{
			out << ch;
			//ungetc(ch, FIN);
			Word[1] = '\0';
			lastsym = 51;
			return 51;//	=
		}
	case'!':
		if ((ch = in.get()) == '=')
		{
			out << ch;
			//putc(ch, FOUT);
			Word[2] = '\0';
			lastsym = 48;
			return 48;//	!=
		}
		else
		{		
			out << ch;
			//不存在单独的!作为操作符
			//ungetc(ch, FIN);
			Word[1] = '\0';
			errorline[errornum] = linenum;
			error[errornum++] = 2;
		}
	}
	//其他情况都错误
	errorline[errornum] = linenum;
	error[errornum++] = 2;
	return 0;//出错
}


//分析主程序
int program()
{
	int haserror = 0;
	linenum = 1;
	hasprintf = false;
	hasscanf = false;
	NumOfC = 0;
	//asmorder = 0;
	oprnum = 0;					//四元式数
	order = 0;					//四元式变量下标
	lev = 0;
	stringorder = 0;				//存放字符串标号
	lableorder = 0;				//标号下标变量
	adr = 0;
	errornum = 0;
	type = 0;
	snum = 0;
	start;
	breakopr, Isbreak = 0;
	typeofexp1 = 11, typeofexp2 = 12;
	oprnumcase = 0;
	Isret = 0;

	globalvar_end = 0;

	char c;
	cout << linenum << "\t";
	//printf("%d:\t", linenum);//输出行号
	sym = getsym();//读入单词
	if (sym == 10)//常量声明
	{

		//＜常量说明部分＞ ::=  const＜常量定义＞｛,＜常量定义＞｝;
		if (constdeclaration())//返回1出错
		{
			//跳过一条语句
			while ((c = in.get()) != '\n')
				out << c;
				//putc(c, FOUT);
			linenum++;
			printf("\n%d:\t", linenum);
			sym = getsym();
			haserror = 1;
		}
	}
	if (sym == 11)//int　变量声明和子函数声明
	{
		int i = sym;
		sym = getsym();
		if (sym == 1)//标识符
		{
			name[NumOfC] = Word;
			//strcpy_s(name[NumOfC], Word);//存入标识符
			sym = getsym();
			if (sym != 39 && sym != 38 && sym != 37)//后面要么是;要么是,(变量声明)要么是(函数声明)
			{
				errorline[errornum] = linenum;
				error[errornum++] = 22;//缺少结束符
				//跳过一条语句
				while ((c = in.get()) != '\n')
					out << c;
					//putc(c, FOUT);
				linenum++;
				printf("\n%d:\t", linenum);
				haserror = 1;
				sym = getsym();
			}

			if (sym == 38 || sym == 39)//如果是变量声明,(; ,)
			{
				if (vardefine(11))
					//变量声明错误处理,输入的是int类型
				{
					//跳过一条语句
					while ((c = fgetc(FIN)) != '\n')
						out << c;
						//putc(c, FOUT);
					linenum++;
					printf("\n%d:\t", linenum);
					haserror = 1;
					sym = getsym();
				}
			}
			globalvar_end = NumOfC;
			if (sym == 37)//如果是子函数声明,左括号
			{
				if (returnfunction(11))//有int类型返回的
				{
					//跳过一条语句
					while ((c = in.get()) != '\n')
						out << c;
						//putc(c, FOUT);
					linenum++;
					printf("\n%d:\t", linenum);
					haserror = 1;
					sym = getsym();
				}
				if (sym == 14)//main,一个里程碑
				{
					if (mainfunction())
					{
						while ((c = in.get()) != '\n')
							out << c;
							//putc(c, FOUT);
						linenum++;
						printf("\n%d:\t", linenum);
						haserror = 1;
					}
					/*	else
							isright=true;*/
				}

				else
				{
					errorline[errornum] = linenum;
					error[errornum++] = 15;//无主函数
					while ((c = in.get()) != '\n')
						out << c;
						//putc(c, FOUT);
					linenum++;
					printf("\n%d:\t", linenum);
					haserror = 1;
				}
			}
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//应为标识符
			while ((c = in.get()) != '\n')
				out << c;
				//putc(c, FOUT);
			linenum++;
			printf("\n%d:\t", linenum);
			haserror = 1;
		}
	}
	if (sym == 13)//void,无返回变量声明
	{
		sym = getsym();
		if (sym == 1)
		{
			if (voidfunction())
			{
				while ((c = in.get()) != '\n')
					out << c;
					//putc(c, FOUT);
				linenum++;
				printf("\n%d:\t", linenum);
				haserror = 1;
			}
			if (sym == 14)//main,一个里程碑
			{
				if (mainfunction())
				{
					while ((c = in.get()) != '\n')
						out << c;
						//putc(c, FOUT);
					linenum++;
					printf("\n%d:\t", linenum);
					haserror = 1;
				}
				/*else
					return 0;*/
			}
			else
			{
				errorline[errornum] = linenum;
				error[errornum++] = 15;//无主函数
				while ((c = in.get()) != '\n')
					out << c;
					//putc(c, FOUT);
				linenum++;
				printf("\n%d:\t", linenum);
				haserror = 1;
			}
		}
		else
			if (sym == 14)
			{
				if (mainfunction())
				{
					while ((c = in.get()) != '\n')
						out << c;
						//putc(c, FOUT);
					linenum++;
					printf("\n%d:\t", linenum);
					haserror = 1;
				}
				/*else
					return 0;*/
			}
	}
	return haserror;

}

//分析常量说 明部分  
int constdeclaration()
{
	//＜常量定义＞    ::=   ＜标识符＞＝＜整数＞
	if (!constdefine())//无误
	{
		if (sym == 38)//;,一句结束
		{
			if ((sym = getsym()) == 10)//再读到const,重新开始
			{
				sym = getsym();
				return(constdeclaration());
			}
			else
				return 0;//无误,返回
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 22;//缺少;
			return 1;
		}
	}
	else
		return 1;
}



//分析常量定义
int constdefine()
{
	//＜常量定义＞          ::=   ＜标识符＞＝＜整数＞
	if ((sym = getsym()) == 1)//是标识符
	{
		name[NumOfC] = Word;
		//strcpy_s(name[NumOfC], Word);//存标识符
		if ((sym = getsym()) == 51)//为=号
		{
			sym = getsym();
			if (sym == 2)//类型检查相符
			{
				if (enter(lev, 11, 4))//写符号表
					return 1;
				if ((sym = getsym()) == 39)//是,号,再进行一次常量定义分析
					return(constdefine());
			}
			else
			{
				errorline[errornum] = linenum;
				error[errornum++] = 28;//变量的类型和值的类型不匹配
				return 1;
			}
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 9;//此处应有'='
			return 1;
		}
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 27;//此处应为标识符
		return 1;
	}
	return 0;//无误
}



//分析变量说明部分  
//这个函数是已经读了类似int a;或int a,才调用的
int vardefine(int type)
{
	if (enter(lev, type, 0))//写符号表,类型其实只有int
		return 1;
	if (sym == 39)//逗号
	{
		if ((sym = getsym()) == 1)//标识符
		{
			name[NumOfC] = Word;
			//strcpy_s(name[NumOfC], Word);//保存标识符
			sym = getsym();
			return(vardefine(type));//type传递下去
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//此处应为标识符
			return 1;
		}
	}
	if (sym == 38)//分号
	{
		sym = getsym();
		if (sym == 11)//int类型,仍是变量声明,识别
		{
			int i = sym;
			if ((sym = getsym()) == 1)
			{
				name[NumOfC] = Word;
				//strcpy_s(name[NumOfC], Word);//存入标识符
				sym = getsym();
				if (sym == 39 || sym == 38)//分号逗号
				{
					if (vardefine(i))
						return 1;
				}
				else if (sym == 37)//左括号,子函数,回去验证
					return 0;
				else
				{
					errorline[errornum] = linenum;
					error[errornum++] = 22;//缺少结束符
					return 1;
				}
			}
			else
			{
				errorline[errornum] = linenum;//应为标识符
				error[errornum++] = 27;
				return 1;
			}
		}
		else
			return 0;
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 22;//缺少结束符
		return 1;
	}
	return 0;
}



//把变量填入符号表
int enter(int lev, int type, int kind)
{
	if (NumOfC < MAXC - 1)//当前元素小于符号表最大元素数
	{
		if (charcheck() && kind != 3)//检查符号表是否有重名
		{
			errorline[errornum] = linenum;
			error[errornum++] = 8;//符号重复定义
			return 1;
		}
		Char[NumOfC].lev = lev;//层数
		Char[NumOfC].kind = kind;//内部类型编码
		Char[NumOfC].type = type;//返回值类型或变量类型
		Char[NumOfC].name = name[NumOfC];//标识符名字

		if (kind == 0 || kind == 3)//参数或变量,当前变量地址加1
		{
			Char[NumOfC].adr = adr;
			adr += 4;
		}

		if (kind == 4)//常量
		{
			if (type == 11)
				Char[NumOfC].num = num;
		}
		NumOfC++;
	}
	else
	{
		errorline[errornum] = linenum;//符号表出界
		error[errornum++] = 7;
		return 1;
	}
	return 0;
}



//检查参数
int charcheck()
{
	int i;
	for (i = NumOfC - 1; i >= 0; i--)
	{
		if (Char[i].lev == lev)
			if (!strcmp(Char[i].name, name[NumOfC]))//不能有重名
				return 1;
	}
	return 0;
}



//在符号表中查找标识符是否存在,存在不冲突的返回1,不存在返回-1
int find()
{
	int i, j = 1;
	for (i = NumOfC - 1; i >= 0; i--)
	{
		if (Char[i].lev == 0 || Char[i].lev == lev)//0层的都可以访问,本层的可以
			if (!strcmp(Char[i].name, Word))
				return i;
	}
	return -1;
}



//语句序列
//＜语句序列＞    ::=  ＜语句＞｛＜语句＞｝
int statementArray()
{
	int a = 0;
	while (sym == 1 || sym == 15 || sym == 18 ||
		sym == 40 || sym == 19 || sym == 20 || sym == 21 || sym == 38)//标识符或者if或者while,scanf,printf,return,左大括号,分号
	{
		if ((a = statement()) == 1)
			return 1;
	}
	return 0;
}



//语句
//＜语句＞ ::=  ＜条件语句＞｜＜循环语句＞｜‘{’<语句序列>‘}’｜＜子函数调用语句＞; ｜＜赋值语句＞; | <返回语句>;｜＜读语句＞;｜＜写语句＞;｜;
int statement()
{
	int a;
	switch (sym) {

	case 15:	//if
		return(ifstatement());
	case 18:	//while
		return(whilestatement());
	case 19:	//scanf
		return(scanfstatement());
	case 20:	//printf
		return(printfstatement());
	case 21:	//return

		return(returnstatement());//return声明
	case 1://标识符
		if ((a = find()) == -1)
		{
			errorline[errornum] = linenum;
			error[errornum++] = 20;
			return 1;
		}
		sym = getsym();
		if (Char[a].kind == 0 || Char[a].kind == 3)//复合语句首先读到了一个标识符,确认是参数或变量
		{
			if (sym == 51)//读到等号
				return(assignstatement(Char[a].name));//赋值语句
			else
			{
				errorline[errornum] = linenum;
				error[errornum++] = 9;
				return 1;
			}
		}
		else if (Char[a].kind == 1 || Char[a].kind == 2)//读到了一个函数名,确定是有返回值或无返回值
		{
			if (sym == 37)//左括号
			{
				if (call(a))
					return 1;
				if (sym == 38)//右括号
					sym = getsym();
				else
				{
					errorline[errornum] = linenum;
					error[errornum++] = 22;
					return 1;
				}
				return 0;
			}
			else
			{
				errorline[errornum] = linenum;
				error[errornum++] = 17;
				return 1;
			}
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 10;
			return 1;
		}
	case 40://左大括号
		sym = getsym();
		if ((a = statementArray()) == 1)
			return 1;
		if (sym != 35)
		{
			errorline[errornum] = linenum;
			error[errornum++] = 12;
			return 1;
		}
		sym = getsym();
		return a;
	case 38:
		sym = getsym();
		return 1;
	default:
		return 1;
	}
	return 1;
}


//表达式

int expression()
{
	bool anti = false;
	char tempresult[40] = "";
	//＜表达式＞            ::=  ［＋｜－］＜项＞｛＜加法运算符＞＜项＞｝
	if (sym == 31 || sym == 32)//带有符号＋、－
	{
		if (sym == 32)//-号
		{
			anti = true;//为负数
		}
		sym = getsym();
	}

	if (term())//项
	{
		return 1;
	}

	if (anti)
	{
		gen("-", re, "", "");//求反
	}

	while (sym == 31 || sym == 32)//
	{
		strcpy_s(tempresult, re);
		int a = sym;
		sym = getsym();
		if (term())//返回值是re
			return 1;
		gettempname();
		if (a == 31)
			gen("ADD", tempresult, re, tempname);
		else
			gen("SUB", tempresult, re, tempname);
		strcpy_s(re, tempname);
		/*if(typeofexp2>typeofexp1)
		typeofexp1=typeofexp2;*/
	}

	return 0;
}



//项
int term()
//＜项＞                ::=   ＜因子＞{＜乘法运算符＞＜因子＞}
{
	char tempresult[40];
	if (factor())
		return 1;

	while (sym == 33 || sym == 34)//×或÷
	{
		strcpy_s(tempresult, re);//返回值先保存
		int tempsym = sym;
		sym = getsym();
		if (factor())
			return 1;
		//识别出两个因子了
		gettempname();//再获得一个
		if (tempsym == 33)
		{
			gen("MUL", tempresult, re, tempname);// *,t1,t2,t3
		}
		else
		{
			gen("DIV", tempresult, re, tempname);// /,t1,t2,t3
		}
		strcpy_s(re, tempname);//返回之
		/*if(typeofexp2>typeofexp1)
		typeofexp1=typeofexp2;*/
	}
	return 0;
}



//因子
//＜因子＞   ::=  ＜标识符＞｜‘（’＜表达式＞‘）’｜＜整数＞｜＜子函数调用语句＞
int factor()
{
	//typeofexp1=11;//int
	bool hassamename = false;
	if (sym == 1)//标识符,包括了子函数调用语句n
	{
		int a = find();//查找表示符是否存在
		//如果在全局区找到,则是全局引用,不变
		//如果在非全局找到,则是局部引用,然后再找全局,如果没有,不变,有,加_
		if (a < globalvar_end)
		{
		}
		else
		{
			for (int i = 0; i < globalvar_end; i++)
			{
				if (!strcmp(Char[i].name, Word))
				{
					hassamename = true;
				}
			}
		}
		if (a == -1)
		{
			errorline[errornum] = linenum;
			error[errornum++] = 20;//标识符未定义
			return 1;
		}

		if (Char[a].kind == 4)//常量,从符号表里读其类型
		{
			if (Char[a].type == 11)//int型常量,直接读取其值
				ss << Char[a].num;
				ss >> re;
				//itoa(Char[a].num, re, 10);//返回一个int值
			sym = getsym();
		}
		else if (Char[a].kind == 0 || Char[a].kind == 3)//变量或者参数
		{
			strcpy_s(re, Word);//返回变量名
			if (hassamename)
				strcat_s(re, "_");
			sym = getsym();
		}
		else if (Char[a].kind == 2)//有返回值的函数
		{
			if ((sym = getsym()) == 37)//(如果下一个是左括号
			{
				if (!call(a))//调用
					return 0;
				else
				{
					errorline[errornum] = linenum;
					error[errornum++] = 17;//缺少左括号
					return 1;
				}
			}
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 11;//无返回值函数不能用在此处
			return 1;
		}
		return 0;
	}
	else if (sym == 37)//左括号
	{
		sym = getsym();
		if (expression())//再次进入表达式
			return 1;
		if (sym != 36)//)右括号
		{
			errorline[errornum] = linenum;
			error[errornum++] = 18;//缺少右括号
			return 1;
		}
		else
			sym = getsym();
		return 0;
	}
	else if (sym == 2)//整数
	{
		strcpy_s(re, Word);//返回这个整数
		sym = getsym();
		return 0;
	}
	/*else if(sym==4)
	{
	if(gen(1,11,cischar))
	return 1;
	sym=getsym();
	return 0;
	}*/
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 6;//缺少运算的因子
		return 1;
	}
	return 0;
}





//子函数调用
int call(int a)
{
	char params[20][20];
	char result[20];
	int i_params = 0;
	int i;
	int count = 0;//参数个数
	sym = getsym();
	for (i = a + 1; i < MAXC; i++)
	{
		if (Char[i].kind != 3)//参数,枚举所有的参数
			break;
		count++;
		if (expression())//返回的是表达式的值
			return 1;
		//保存参数
		strcpy_s(params[i_params++], re);
		if (sym == 39)//,
			sym = getsym();
		else
			break;
	}
	for (int j = 0; j < i_params; j++)
	{
		if (gen("param", params[j], "", ""))
			return 1;//压入参数
	}

	//itoa(count, result, 10);
	ss << count;
	ss >> result;
	if (gen("call", Char[a].name, ",", result))//call test,2
		return 1;
	if (Char[i].kind != 3 && sym != 36 || Char[i + 1].kind == 3 && sym == 36)//前一种传入参数多了，后一种传少了
	{
		if (count != 0)
		{
			//无参情况
			errorline[errornum] = linenum;
			error[errornum++] = 16;//函数调用参数不对
			return 1;
		}
	}

	if (sym == 36)//右括号
	{
		sym = getsym();
		gettempname();
		gen("receive", tempname, "", "");//receive t1;
		strcpy_s(re, tempname);//返回临时单元
		return 0;
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 18;
		return 1;
	}
	return 0;
}




//有返回值的函数
int returnfunction(int type)
{

	adr = 0;
	char currentprocname[40];//当前函数名
	//已经识别到左括号了
	Isret = 1;//有无返回值
	if (enter(0, type, 2))//函数定义写入符号表,函数置为0级
		return 1;
	strcpy_s(currentprocname, Char[NumOfC - 1].name);
	//生成代码,PROC 函数名
	gen("proc", currentprocname, "", "");
	lev++;
	sym = getsym();
	//判断参数
	while (sym == 11/*||sym==12*/)//参数声明,int型参数
	{
		//int i=sym;
		if ((sym = getsym()) == 1)//参数名字
		{
			strcpy_s(name[NumOfC], Word);
			if (enter(lev, 11, 3))//参数写入符号表
				return 1;
			//读取一个参数
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//应为标识符
			return 1;
		}
		if ((sym = getsym()) != 39)//不是逗号
			break;
		sym = getsym();
		if (sym != 11/*&&sym!=12*/)//int
		{
			errorline[errornum] = linenum;
			error[errornum++] = 4;//缺少类型标识符
			return 1;
		}
	}
	if (sym == 36)//右括号
	{
		if ((sym = getsym()) == 40)//左大括号,开始复合语句啦
		{
			sym = getsym();
			//＜复合语句＞::=‘｛’［＜常量说明部分＞］［＜变量说明部分＞］＜语句序列＞‘｝’
			//常量说明
			if (sym == 10)//const
			{
				//sym=getsym();
				if (constdeclaration())//const声明
					return 1;
			}
			//变量说明int a;			
			if (sym == 11/*||sym==12*/)
			{
				int i = sym;
				if ((sym = getsym()) == 1)//标识符
				{
					strcpy_s(name[NumOfC], Word);
					sym = getsym();
					if (sym != 38 && sym != 39)//不等于分号逗号
					{
						errorline[errornum] = linenum;
						error[errornum++] = 22;//缺少分号逗号
						return 1;
					}
					if (sym == 38 || sym == 39)//等于分号或逗号
					{
						if (vardefine(11))
							return 1;
					}
				}
				else
				{
					errorline[errornum] = linenum;//应为标识符
					error[errornum++] = 27;
					return 1;
				}
			}
			//前面都是变量定义

			//语句序列
			if (statementArray())
				return 1;

		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 23;//缺少左大括号
			return 1;
		}
		//上述函数最后又读了一位
		if (sym == 35)//右大括号
		{
			sym = getsym();
			//backout();//编译完成退出不用的符号
			gen("end", currentprocname, "", "");
			if (sym == 11)//int
			{
				int c;
				sym = getsym();
				if (sym == 1)//标识符
				{
					strcpy_s(name[NumOfC], Word);
					if ((sym = getsym()) == 37)//左括号,又一个函数声明
						return returnfunction(11);
					else
					{
						errorline[errornum] = linenum;
						error[errornum++] = 17;//缺左括号
						while ((c = fgetc(FIN)) != '\n')
							putc(c, FOUT);
						linenum++;
						printf("\n%d:\t", linenum);
						return 1;
					}
				}
				else
				{
					errorline[errornum] = linenum;
					error[errornum++] = 27;//应为标识符
					while ((c = fgetc(FIN)) != '\n')
						putc(c, FOUT);
					linenum++;
					printf("\n%d:\t", linenum);
					return 1;
				}
			}
			else
				if (sym == 13)//void
				{
					if ((sym = getsym()) == 1)
						return(voidfunction());
				}
				else
					if (sym == 14)
					{
						return 0;
					}
			//lev--;//减一层
			return 0;
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 12;//缺少右大括号
			return 1;
		}
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 18;//缺少右括号
		return 1;
	}
}


//无返回值的函数定义语句
int voidfunction()
{
	char currentprocname[40];
	Isret = 0;
	strcpy_s(name[NumOfC], Word);
	if (enter(0, 0, 1))//写入符号表,函数置为0级
		return 1;
	strcpy_s(currentprocname, Word);
	gen("proc", currentprocname, "", "");
	lev++;
	if ((sym = getsym()) != 37)//左括号
	{
		errorline[errornum] = linenum;
		error[errornum++] = 17;//缺左括号
		return 1;
	}
	sym = getsym();
	while (sym == 11/*||sym==12*/)//参数
	{
		int i = sym;
		if ((sym = getsym()) == 1)//变量名称
		{
			strcpy_s(name[NumOfC], Word);
			if (enter(lev, i, 3))//写入,这是一个参数
				return 1;
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//此处应为标识符
			return 1;
		}
		if ((sym = getsym()) != 39)//不是逗号,退出
			break;
		sym = getsym();

		if (sym != 11/*&&sym!=12*/)//不等于int
		{
			errorline[errornum] = linenum;
			error[errornum++] = 4;//缺少类型标识符
			return 1;
		}
	}
	if (sym == 36)//右括号
	{
		if ((sym = getsym()) == 40)//左大括号
		{
			sym = getsym();
			if (sym == 10)//const
			{
				//				sym=getsym();
				if (constdeclaration())
					return 1;
			}
			if (sym == 11/*||sym==12*/)//变量定义
			{
				int i = sym;
				if ((sym = getsym()) == 1)//标识符
				{
					strcpy_s(name[NumOfC], Word);
					sym = getsym();
					if (sym != 38 && sym != 39)//分号逗号
					{
						errorline[errornum] = linenum;
						error[errornum++] = 22;
						return 1;
					}
					if (sym == 38 || sym == 39)//分号逗号
						if (vardefine(i))
							return 1;
				}
				else {
					errorline[errornum] = linenum;
					error[errornum++] = 27;
					return 1;
				}
			}

			if (statementArray())//语句序列
				return 1;

		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 23;
			return 1;
		}
		if (sym == 35)//右大括号复合语句结束
		{
			//backout();//退出
			//lev--;//减一层
			//再次进入voidfunction循环
			gen("end", currentprocname, "", "");
			if ((sym = getsym()) == 13)//void
			{
				if ((sym = getsym()) == 1)
					return(voidfunction());
			}
			else
				if (sym == 14)//main函数
					return 0;
				else
					if (sym == 11)//int　子函数声明
					{
						int c;
						sym = getsym();
						if (sym == 1)//标识符
						{
							strcpy_s(name[NumOfC], Word);
							if ((sym = getsym()) == 37)//左括号,又一个函数声明
								return returnfunction(11);
							else
							{
								errorline[errornum] = linenum;
								error[errornum++] = 17;//缺左括号
								while ((c = fgetc(FIN)) != '\n')
									putc(c, FOUT);
								linenum++;
								printf("\n%d:\t", linenum);
								return 1;
							}
						}
						else
						{
							errorline[errornum] = linenum;
							error[errornum++] = 27;//应为标识符
							while ((c = fgetc(FIN)) != '\n')
								putc(c, FOUT);
							linenum++;
							printf("\n%d:\t", linenum);
							return 1;
						}
					}
					else
					{
						errorline[errornum] = linenum;
						error[errornum++] = 15;//无主函数
						return 1;
					}

			return 0;
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 12;//缺右大括号
			return 1;
		}
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 18;//缺右括号
		return 1;
	}
	return 0;
}



//赋值语句
int assignstatement(char* left)
{
	bool hassamename = false;
	sym = getsym();
	if (expression())
		return 1;
	strcpy_s(Word, left);
	int a = find();//查找表示符是否存在
	//如果在全局区找到,则是全局引用,不变
	//如果在非全局找到,则是局部引用,然后再找全局,如果没有,不变,有,加_
	if (a < globalvar_end)
	{
	}
	else
	{
		for (int i = 0; i < globalvar_end; i++)
		{
			if (!strcmp(Char[i].name, Word))
			{
				hassamename = true;
			}
		}
	}
	if (hassamename)
		strcat_s(Word, "_");
	gen(Word, ":=", re, "");
	if (sym != 38)//缺少;
	{
		errorline[errornum] = linenum;
		error[errornum++] = 22;
		return 1;
	}
	sym = getsym();
	return 0;
}



//if条件语句
//＜条件语句＞          ::=   if‘（’＜条件＞‘）’＜语句＞［else＜语句＞］
int ifstatement()
{
	char templabel[10] = "", beforelabel[10] = "", afterlabel[10] = "", middlelabel[10] = "";
	//用来存放临时标号,之前标号,之后标号,中间标号

	if ((sym = getsym()) == 37)//左括号
	{
		char symbol[4];//存运算符
		char tempresult[40];//存第一个的返回结果
		sym = getsym();
		//＜条件＞ ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
		if (expression()) //表达式,返回值在re
			return 1;
		strcpy_s(tempresult, re);
		getnewlabel(templabel);//获得一个新的标号,放在templabel里

		if (sym == 45 || sym == 46 || sym == 47 || sym == 48 || sym == 49 || sym == 50)//关系运算符
		{
			int b = sym;
			sym = getsym();
			if (expression())//表达式,返回值在re
				return 1;
			switch (b)
			{
			case 45://>
				strcpy_s(symbol, ">");
				break;
			case 46://<
				strcpy_s(symbol, "<");
				break;
			case 47://==
				strcpy_s(symbol, "==");
				break;
			case 48://!=
				strcpy_s(symbol, "!=");
				break;
			case 49://>=
				strcpy_s(symbol, ">=");
				break;
			case 50://<=
				strcpy_s(symbol, "<=");
				break;
			}
		}
		gen("if", tempresult, symbol, re);

		if (sym != 36)//右括号
		{
			errorline[errornum] = linenum;
			error[errornum++] = 18;//少右括号
			return 1;
		}

		gen("goto", templabel, "", "");
		strcpy_s(beforelabel, templabel);
		getnewlabel(templabel);//获得一个新的标号,放在templabel里
		gen("goto", templabel, "", "");
		strcpy_s(afterlabel, templabel);
		gen(beforelabel, ":", "", "");
		sym = getsym();
		if (statement() == 1)
			return 1;

		if (sym == 16)//else
		{
			getnewlabel(templabel);//获得一个新的标号,放在templabel里
			gen("goto", templabel, "", "");
			gen(afterlabel, ":", "", "");
			sym = getsym();
			if (statement() == 1)//语句
				return 1;
			gen(templabel, ":", "", "");
		}
		else //无else
		{
			gen(afterlabel, ":", "", "");
		}


	}
	else {
		errorline[errornum] = linenum;
		error[errornum++] = 17;//少左括号
		return 1;
	}
	return 0;
}



//while循环语句
//＜循环语句＞          ::=   while‘（’＜条件＞‘）’＜语句＞
int whilestatement()
{
	char templabel[10] = "", beforelabel[10] = "", afterlabel[10] = "", middlelabel[10] = "";
	//用来存放临时标号,之前标号,之后标号,中间标号
	char symbol[20] = "";//存运算符
	char resulta[20] = "";//存第一个的返回值
	//int oprnum1=oprnum,oprnum2,oprnum4[MAXCASE],a,j=0;
	if ((sym = getsym()) == 37)//左括号
	{
		//＜条件＞              ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
		sym = getsym();
		if (expression()) //表达式
			return 1;
		getnewlabel(templabel);//获得一个新的标号,放在templabel里,l0
		gen(templabel, ":", "", "");
		strcpy_s(beforelabel, templabel);
		getnewlabel(templabel);//获得一个新的标号,放在templabel里,l1
		strcpy_s(resulta, re);
		if (sym == 45 || sym == 46 || sym == 47 || sym == 48 || sym == 49 || sym == 50)//关系运算符
		{
			int b = sym;
			sym = getsym();
			if (expression())//表达式,返回值在re
				return 1;
			switch (b)
			{
			case 45://>
				strcpy_s(symbol, ">");
				break;
			case 46://<
				strcpy_s(symbol, "<");
				break;
			case 47://==
				strcpy_s(symbol, "==");
				break;
			case 48://!=
				strcpy_s(symbol, "!=");
				break;
			case 49://>=
				strcpy_s(symbol, ">=");
				break;
			case 50://<=
				strcpy_s(symbol, "<=");
				break;
			}
		}

		gen("if", resulta, symbol, re);//if a>b
		gen("goto", templabel, "", "");//goto tempabel
		strcpy_s(middlelabel, templabel);
		getnewlabel(templabel);//获得一个新的标号,放在templabel里
		gen("goto", templabel, "", "");//goto l2	
		strcat_s(afterlabel, templabel);
		gen(middlelabel, ":", "", "");


		if (sym != 36)
		{
			errorline[errornum] = linenum;
			error[errornum++] = 18;//少右括号
			return 1;
		}
		sym = getsym();
		if (statement())//语句(大括号里的)
			return 1;
		gen("goto", beforelabel, "", "");
		gen(afterlabel, ":", "", "");
	}
	else {
		errorline[errornum] = linenum;
		error[errornum++] = 17;//少左括号
		return 1;
	}
	return 0;
}




//scanf语句
//＜读语句＞            ::=  scanf‘(’＜标识符＞‘）’
int scanfstatement()
{
	int a;
	bool hassamename = false;
	sym = getsym();
	if (sym == 37)//左括号
	{
		sym = getsym();//标识符
		if (sym != 1)
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//应为标识符
			return 1;
		}
		a = find();//查符号表
		//如果在全局区找到,则是全局引用,不变
		//如果在非全局找到,则是局部引用,然后再找全局,如果没有,不变,有,加_
		if (a < globalvar_end)
		{
		}
		else
		{
			for (int i = 0; i < globalvar_end; i++)
			{
				if (!strcmp(Char[i].name, Word))
				{
					hassamename = true;
				}
			}
		}
		if (a == -1)
		{
			errorline[errornum] = linenum;
			error[errornum++] = 20;//标识符未定义就使用了
			return 1;
		}
		if (Char[a].kind == 0 || Char[a].kind == 3)//函数里的形参参数或者变量
		{
			hasscanf = true;
			if (hassamename)
			{
				char temp[10];
				strcpy_s(temp, Char[a].name);
				strcat_s(temp, "_");
				gen("in", temp, "", "");
			}
			else
			{
				gen("in", Char[a].name, "", "");
			}

		}
		if ((sym = getsym()) != 36)//右括号
		{
			errorline[errornum] = linenum;
			error[errornum++] = 18;//应为右括号
			return 1;
		}
		if ((sym = getsym()) == 38)//分号
		{
			sym = getsym();
			return 0;
		}
		else {
			errorline[errornum] = linenum;
			error[errornum++] = 22;//少分号
			return 1;
		}

	}
	errorline[errornum] = linenum;
	error[errornum++] = 17;//缺少左括号
	return 0;
}


//printf语句
//＜写语句＞   ::=  printf‘(’<字符串>,＜表达式 ＞|<字符串>|＜表达式 ＞‘）’
int printfstatement()
{

	int	j = 0;
	int	a = 0;

	if ((sym = getsym()) == 37)//左括号
	{
		sym = getsym();
		if (sym == 5)//字符串
		{
			hasprintf = true;
			gen("out", storestring[stringorder - 1].stringname, "", "");//out STR0	
			sym = getsym();//读下一个
			if (sym == 39)//逗号
			{
				sym = getsym();//读下一个
				if (expression())//标识符
					return 1;
				//printf("sdfs",a);结构
				gen("out", re, "", "");//输出表达式结果
				//					sym=getsym();//读下一个

			}
			else if (sym = 36)//右括号
			{
				//printf("helloworld")类型结束			
			}
			else
			{
				errorline[errornum] = linenum;
				error[errornum++] = 18;//缺少右括号
				return 1;
			}
		}
		else if (!expression()) //标识符
		{
			hasprintf = true;
			gen("out", re, "", "");//输出表达式结果
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//缺少分号
			return 1;
		}

		if (sym == 36)//右括号
		{
			sym = getsym();//读下一个
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 18;//缺少右括号
			return 1;
		}

		if (sym == 38)
		{
			sym = getsym();
		}
		else {
			errorline[errornum] = linenum;
			error[errornum++] = 22;//缺少分号
			return 1;
		}
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 17;//应为左括号
		return 1;
	}
	return 0;

}

//return语句
int returnstatement()
{
	if ((sym = getsym()) == 37)//左括号,有返回值的函数
	{
		if (!Isret)//无返回值函数 
		{
			errorline[errornum] = linenum;
			error[errornum++] = 14;//少分号
			return 1;
		}
		sym = getsym();
		if (expression())//表达式
			return 1;
		gen("return", re, "", "");//return c;
		if (sym == 36)//右括号
		{
			if ((sym = getsym()) == 38)//分号
			{
				sym = getsym();//语法正确
				return 0;
			}
			else {
				errorline[errornum] = linenum;
				error[errornum++] = 22;//少分号
				return 1;
			}
		}
		else {
			errorline[errornum] = linenum;//少括号
			error[errornum++] = 18;
			return 1;
		}
	}
	if (sym == 38)//分号,无返回值的函数
	{
		if (Isret)//有返回值函数
		{
			errorline[errornum] = linenum;
			error[errornum++] = 29;//少分号
			return 1;
		}
		sym = getsym();
		return 0;
	}
	errorline[errornum] = linenum;
	error[errornum++] = 22;//缺分号

	return 1;
}



//main函数处理
int mainfunction()
{
	Isret = 0;
	start = oprnum;
	strcpy_s(name[NumOfC], Word);
	if (enter(0, 0, 5))//函数置为0级
		return 1;
	lev++;
	Char[NumOfC - 1].adr = oprnum;
	if ((sym = getsym()) != 37)//左括号
	{
		errorline[errornum] = linenum;
		error[errornum++] = 17;//缺少左括号
		return 1;
	}
	sym = getsym();
	//while(sym==11/*||sym==12*/)//int
	//{
	//	int i=sym;
	//	if((sym=getsym())==1)//标识符
	//	{
	//		strcpy(name[NumOfC],Word);
	//		if(enter(lev+1,i,3))
	//			return 1;
	//	}
	//	else
	//	{
	//		errorline[errornum]=linenum;
	//		error[errornum++]=27;
	//		return 1;
	//	}
	//	if((sym=getsym())!=39)
	//		break;
	//	sym=getsym();

	//	if(sym!=11&&sym!=12)
	//	{
	//		errorline[errornum]=linenum;
	//		error[errornum++]=4;
	//		return 1;
	//	}
	//}
	if (sym == 36)//右括号
	{
		if ((sym = getsym()) == 40)//左大括号
		{
			gen("proc", "main", "", "");//main函数开始
			sym = getsym();
			if (sym == 10)//const
			{
				//sym=getsym();
				if (constdeclaration())
					return 1;
			}
			if (sym == 11/*||sym==12*/) //变量声明
			{
				int i = sym;
				if ((sym = getsym()) == 1)//标识符
				{
					strcpy_s(name[NumOfC], Word);
					sym = getsym();
					if (sym != 39 && sym != 38)//;,
					{
						errorline[errornum] = linenum;
						error[errornum++] = 22;
						return 1;
					}
					if (sym == 39 || sym == 38)
						if (vardefine(i))
							return 1;

				}
				else {
					errorline[errornum] = linenum;
					error[errornum++] = 27;
					return 1;
				}
			}


			if (statementArray())
				return 1;
			//判断最后的大括号
			if (sym == 35)
			{
				gen("end", "main", "", "");//main函数结束
				return 0;
			}
			else
			{
				errorline[errornum] = linenum;
				error[errornum++] = 12;
				return 1;
			}

		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 23;
			return 1;
		}
		if (sym == 35)//右大括号
		{
			//backout();
			//lev--;两边编译不退栈了,直接一个函数一个lev,最前面的全局定义为0,所有人都可以访问,其他的不可以
			Isret = 0;
			return 0;
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 12;//缺右大括号
			return 1;
		}
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 18;//缺右括号
		return 1;
	}
	Isret = 0;
	return 0;

}

//产生一条表达式
int gen(string op, string src1, string src2, string dst)//op是指令序号,其他三个是符号表序号
{
	if (oprnum < MAXOP)//指令不要超
	{
		opra[oprnum].op = op;
		strcpy_s(opra[oprnum].op, op);
		strcpy_s(opra[oprnum].src1, src1);
		strcpy_s(opra[oprnum].src2, src2);
		strcpy_s(opra[oprnum].dst, dst);
		oprnum++;
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 19;//指令超了
		return 1;
	}
	return 0;
}

void gettempname()//获得临时变量
{
	strcpy_s(tempname, "T");
	char temp[3];
	ss << order++;
	ss >> temp;
	//itoa(order++, temp, 10);
	strcat_s(tempname, temp);
	strcpy_s(name[NumOfC], tempname);

	//临时变量存入符号表
	enter(lev, 11, 0);
}

void getnewlabel(string templabel)
{
	//strcpy_s(templabel, "LP");
	templabel = "LP";
	char temp[5];
	ss << lableorder++;
	ss >> temp;
	//itoa(lableorder++, temp, 10);
	//strcat_s(templabel, temp);
	templabel += temp;
}


void getstringname()//获得新的标号
{
	char temp[3];
	ss << stringorder;
	ss >> temp;
	//itoa(stringorder, temp, 10);
	strcpy_s(storestring[stringorder].stringname, "STR");
	strcat_s(storestring[stringorder].stringname, temp);
}

int getshellsym()//类shell程序词法分析,返回值在
{
	char  c;//输入字符
	int i = 0;//shelltoken字符序号
	//c = getchar();
	cin >> c;
	while (c == '\n' || c == ' ' || c == 9)//跳过无关的
	{
		cin >> c;
	}
	while (c != '\n' && c != ' '&& c != 9)//遇到终止的符号就停止拼合
	{
		//拼合
		shelltoken[i++] = c;
		//c = getchar();
		cin >> c;
	}
	shelltoken[i] = '\0';
	for (i = 0; i <= CommandNum; i++)
	{
		if ((shelltoken==shell[i]) || (strlen(shelltoken) == 1 && shell[i][0] == shelltoken[0]))//相同或只有一个字符且首字符相同
		{
			return i;
		}

	}
	return 10;//作为文件名
}

//主函数
int main()
{
	int c;
	bool hascompileed = false;
	//类shell解释循环
	while (1)
	{
		printf("C0Compile>>");
		c = getshellsym();//获得一个字符
		switch (c)
		{
		case 0://compile或c
		{c = getshellsym();//获得文件名
		if (c == 10)//文件名
		{
			//文件名加扩展名
			if (strchr(shelltoken, '.') == NULL)
				strcat_s(shelltoken, ".txt");
			//FIN = fopen(shelltoken, "r");
			in.open(shelltoken);
			if (!in)
			{
				cout<<"File not found!\n";
				break;
			}
		//LP:
			printf("\n源程序:\n");
			FOUT = stdout;
			if (program()) {
				while ((c = fgetc(FIN)) != EOF)
				{
					if (c == '\n')
					{
						linenum++;
						printf("\n%d:\t", linenum);
					}
					else
						putc(c, FOUT);
				}
				printf("\n错误信息\n");
				int i = errornum;
				for (errornum = 0; errornum < i; errornum++)
				{
					error_msg(error[errornum]);
				}
				break;
			}
			if (errornum == 0)
			{
				printf("\n\n编译通过!\n\n");
				hascompileed = true;
				fclose(FIN);
			}
			break;
		}}
		case 1://link或l
		{
			c = getshellsym();
			{
				if (c == 10)
				{
					int i = 0;
					while (shelltoken[i] != '\0')
					{
						if (shelltoken[i] == '.')
						{
							shelltoken[i] = '\0';
							break;
						}
						i++;
					}
					char temp[40];
					strcpy_s(temp, "go ");
					strcat_s(temp, shelltoken);
					system(temp);
				}
			}
		}
		break;
		case 2://run或r
		{	c = getshellsym();
			char temp[40];
			strcpy_s(temp, shelltoken);
			strcat_s(temp, ".exe");
			system(temp);
			break;
		}
		case 3://quit或q
		{
			if (FIN != NULL)
			fclose(FIN);
			exit(0);
		}
		case 4://help或h
		{
			printf("Help of C0Compile.\nYou can get this help by command compile help or type help/h in command line.\n");
			printf("----------------------------------------------\n");
			printf("Run with parameters:compile filename[.txt]\n");
			printf("Run without parameters:compile\n");
			printf("----------------------------------------------\n");
			printf("Inner command help\n");
			printf("Compile a file: compile/c filename[.txt]\n");
			printf("Link an obj file: link/r\n");
			printf("Run an exe file: run/r\n");
			printf("Change the invoke mode: invoke [stdcall/cdecl]\n");
			printf("Quit from this command line:quit/q\n");
			break;
		}
		case 5://write 
		{
			if (!hascompileed)//没编译
			{
				printf("You shoud first compile it.");
				continue;
			}
			//读取一个文件名
			c = getshellsym();//获得文件名
			if (c == 10)//文件名
			{
				//文件名加扩展名
				if (strchr(shelltoken, '.') == NULL)
					strcat_s(shelltoken, ".txt");
				out.open(shelltoken);
				out.open(shelltoken, ios::in||ios::out);
				//FOUT = fopen(shelltoken, "w");
				int i;
				for (i = 0; i < oprnum; i++)
				{
					//输出行号,操作符名,操作符代码
					fprintf(FOUT, "%s\t%s\t%s\t%s\n", opra[i].op,
						opra[i].src1, opra[i].src2, opra[i].dst);
				}
				//中间代码写入成功
				printf("Code has been write into file successfully.\n");
				fclose(FOUT);
				break;
			}
		}
		//case 6://asm
		/*{
			if (!hascompileed)//没打开文件
			{
				printf("You shoud first compile it.");
				continue;
			}

			//读取一个文件名
			c = getshellsym();//获得文件名
			if (c == 10)//文件名
			{
				//文件名加扩展名
				if (strchr(shelltoken, '.') == NULL)
					strcat(shelltoken, ".asm");
				FOUT = fopen(shelltoken, "w");
				toasm();//生成汇编
				int i;
				for (i = 0; i < asmorder; i++)
				{
					//输出行号,操作符名,操作符代码
					fprintf(FOUT, "%s\n", asmpr[i]);
				}
				//中间代码写入成功
				fclose(FOUT);
				printf("Assembly code has been write into file successfully.\n");
				break;
			}
		}*/
		case 6://print或p
		{
			c = getshellsym();//获得文件名
			if (c == 10)
			{
				//FILE *fin;
				char *one_line; // 读入的一行 
				int buff_size = 120; //根据最长行字符定大小 
				in.open(shelltoken);
				if (!in) {
					printf("can not open file %s\n", shelltoken);
					break;
				};

				one_line = (char *)malloc(buff_size * sizeof(char));

				while (in.eof()) {
					//printf("%s", one_line); // 读一行打一行 
					in.getline(one_line, buff_size);
					cout << one_line;
				};
				in.close();
				break;
			}
		}
		case 7:
		{
			c = getshellsym();
			if (c == 10)//stdcall,cdecl
			{
				if (!strcmp(shelltoken, "stdcall"))
				{
					call_rule = 1;
				}
				else
				{
					call_rule = 0;
				}
				break;
			}
		}
		default:
			printf("Bad commmand or filename!\n");
		}

	}

}