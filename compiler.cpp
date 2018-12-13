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

int linenum = 1;					//Դ�����к�
bool hasprintf = false;			//�Ƿ���printf;
bool hasscanf = false;			//�Ƿ���scanf
int  call_rule = 0;				//���ù���,0ΪC,1Ϊstdcall

string name[MAXWORD];	//���ʶ��
/*1*/
//string name[MAXWORD];
//char asmpr[MAXOP][150];		//����ָ��

int NumOfC = 0;					//���ű���Ԫ�صĸ���
Character Char[MAXC];			//���ű�
opr  opra[MAXOP];				//����Ԫʽ
//int	asmorder = 0;					//���ָ���±����
int oprnum = 0;					//��Ԫʽ��
int order = 0;					//��Ԫʽ�����±�
int lev = 0;						//����

char re[10];					//����������ʱ�������ۺ�����
string tempname;				//���������ʱ������
int errornum = 0, error[MAXERROR], errorline[MAXERROR];//������,������Ϣ����ʹ���������
string trans;//��ӿ�ͷ�ı�ʶ��ת

//char tempstringname[10];		//�����ʱ�ַ�������
int stringorder = 0;				//����ַ������

int lableorder = 0;				//����±����
int adr = 0;						//ÿ�������ڱ���ƫ�Ƶ�ַ
string shelltoken;			//�����ͳ����token

int lastsym = 0;//��һ���ַ�
int type = 0;
int snum = 0;
int start;
int breakopr, Isbreak = 0;
int typeofexp1 = 11, typeofexp2 = 12;
int oprnumcase = 0;
int Isret = 0;

int globalvar_end = 0;//ȫ�ֱ���������λ��


//������
void error_msg(int i)
{
	//fprintf(FOUT, "line(%d)\terror%d:\t\t%s\n", errorline[errornum], i, err_msg[i]);
	if (out.is_open())
	{
		out << "line" << errorline[errornum] << "\terrord" << i << ":\t\t" << err_msg[i] << "\n";
	}
}


//�ַ���ȡ�ʹ���
int getsym()
{

	char ch;//������ַ�				
	int i, j, m;
	//������һ���ǿ��ַ�
		in.get(ch);
		//ch = fgetc(FIN);//����һ���ַ�
	while (isspace(ch))//����ո�
	{
		out << ch;
		//putc(ch, FOUT);//ֱ�����
		if (ch == '\n')
		{
			linenum++;//�кż�1
			printf("%d:\t", linenum);//����к�
		}
		in.get(ch);
		//ch = fgetc(FIN);
	}

	i = 0;//
	Word[i] = ch;//���ӽ�ȥ
	i++;

	//�ж�����ַ�
	//�������ĸ
	if (isalpha(ch) || ch == '_')
	{
		out << ch;
		//putc(ch, FOUT);//���
		in.get(ch);
		//ch = fgetc(FIN);
		while (isalpha(ch) || isdigit(ch) || ch == '_')//������ַ�����
		{
			Word[i] = ch;//����
			i++;
			if (i == MAXWORD)//��ʶ�������ж�
			{
				errorline[errornum] = linenum;//������
				error[errornum++] = 1;
				i--;
				Word[i] = '\0';//����
				//���������Ĳ���
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
				//ungetc(ch, FIN);//����һ��
				lastsym = 1;
				return 1;//�Ǳ�ʶ��
			}
			out << ch;
			in.get(ch);
			//putc(ch, FOUT);
			//ch = fgetc(FIN);
		}
		//����һ��,����
		in.seekg(1, ios::cur);
		//ungetc(ch, FIN);
		//Word[i] = '\0';

		for (m = 0; m < kk; m++)//�������Ƿ�Ϊkeyword
		{
			if (Keyword[m]==Word)	//��ȡ���ַ������϶�key word���ж�
			{
				lastsym = m + 10;
				return m + 10;//������Ӧ��keyword�Ķ�Ӧ����
			}
		}
		if (m == kk) {
			//�����ִ�Сд,��ɴ�д
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
			return 1;//����key word,�Ǳ�ʶ��
		}
	}

	//��ֵ����
	if (isdigit(ch))
	{
		//putc(ch, FOUT);//���
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
				//������������
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
				//num = ;//���ַ���ת��Ϊ��Ӧ������
				//ungetc(ch, FIN);
				in.seekg(1, ios::cur);
				lastsym = 2;
				return 2;//������
			}

		}
		in.seekg(1, ios::cur);
		//ungetc(ch, FIN);
		Word[i] = '\0';
		ss >> Word;
		ss << num;
		//num = atoi(Word);//���ַ���ת��Ϊ��Ӧ������
		lastsym = 2;
		return 2;//������
	}


	//�ֽ��
	i = 34;
	out << ch;
	//putc(ch, FOUT);
	switch (ch) {

		//case'\'':
		//			ch=fgetc(FIN);
		//			if(ch=='+'||ch=='-'||ch=='*'||ch=='\''||isalpha(ch)) 
		//				//��Ӧ��printf����еĸ��ֲ���
		//			{
		//				cischar=ch;
		//				putc(ch,FOUT);
		//				if ((ch=fgetc(FIN))=='\'')
		//				{
		//					putc(ch,FOUT);
		//					return 4;//�ַ�
		//				}
		//				else 
		//				{
		//					putc(ch,FOUT);
		//					errorline[errornum]=linenum;
		//					error[errornum++]=3;
		//					return -1;
		//				}
		//			}

	case'"'://������
		j = 0;
		while ((ch = fgetc(FIN)) != '"')//���Ƿǿմ�,ƴ��
		{
			//putc(ch, FOUT);
			out << ch;
			sisstring[j++] = ch;//ƴ��

			if (j == MAXLENGTH)
			{
				printf("warning!�ַ�������\n");
				sisstring[--j] = '\0';
				//����
				out << ch;
				//putc(ch,FOUT);
				//ch = fgetc(FIN);
				in.get(ch);
				//����
				while (ch != '"')
				{
					out << ch;
					//putc(ch, FOUT);
					j++;
					if (ch == EOF || j > 100)	//�ַ���������ȱ�ٷ�����
					{
						errorline[errornum] = linenum;
						error[errornum++] = 5;
						return -1;
					}
				}
				getstringname();//��ñ��,д����
				//strcpy_s(storestring[stringorder++].sisstring, sisstring);//�����ַ���
				storestring[stringorder++].sisstring = sisstring;
				lastsym = 5;
				return 5;//�����ַ���
			}
		}
		out << ch;
		//putc(ch, FOUT);//������߽��
		sisstring[j] = '\0';//�ַ�����β
		getstringname();//��ñ��,д����
		storestring[stringorder++].sisstring = sisstring;
		//strcpy_s(storestring[stringorder++].sisstring, sisstring);//�����ַ���	
		lastsym = 5;
		return 5;//���ؿ��ַ���
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
		if ((ch = in.get()) == '/')//��Ϊ����ע�ͱ�־
		{
			out << ch;
			//putc(ch, FOUT);
			while ((ch = in.get()) != '\n')
				//putc(ch, FOUT);//ע�����ݲ�����ֱ�����
				cout << ch;
			out << ch;
			//putc(ch, FOUT);
			linenum++;
			printf("%d:\t", linenum);
			return (getsym());//�ݹ���ã�������һ�д���,������goto
		}
		//ungetc(ch, FIN);
		in.seekg(1, ios::cur);
		Word[1] = '\0';
		lastsym = 34;
		return 34; //��Ϊ/
	case'*':
		Word[1] = '\0';
		lastsym = 33;
		return 33;
	case '-':
	case'+':
		if (lastsym == 37 || lastsym == 45 || lastsym == 46 || lastsym == 47 || lastsym == 48 || lastsym == 49 || lastsym == 50 || lastsym == 51)//ʶ��Ϊ����
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
					//putc(ch, FOUT);//ƴ������
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
						return 2;//�����ŵ�����
					}
				}
				//ungetc(ch, FIN);
				in.seekg(1, ios::cur);
				Word[j] = '\0';
				//num = atoi(Word);
				ss >> Word;
				ss << num;
				lastsym = 2;
				return 2;//�����ŵ�����
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
		else			//�������
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
			//�����ڵ�����!��Ϊ������
			//ungetc(ch, FIN);
			Word[1] = '\0';
			errorline[errornum] = linenum;
			error[errornum++] = 2;
		}
	}
	//�������������
	errorline[errornum] = linenum;
	error[errornum++] = 2;
	return 0;//����
}


//����������
int program()
{
	int haserror = 0;
	linenum = 1;
	hasprintf = false;
	hasscanf = false;
	NumOfC = 0;
	//asmorder = 0;
	oprnum = 0;					//��Ԫʽ��
	order = 0;					//��Ԫʽ�����±�
	lev = 0;
	stringorder = 0;				//����ַ������
	lableorder = 0;				//����±����
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
	//printf("%d:\t", linenum);//����к�
	sym = getsym();//���뵥��
	if (sym == 10)//��������
	{

		//������˵�����֣� ::=  const���������壾��,���������壾��;
		if (constdeclaration())//����1����
		{
			//����һ�����
			while ((c = in.get()) != '\n')
				out << c;
				//putc(c, FOUT);
			linenum++;
			printf("\n%d:\t", linenum);
			sym = getsym();
			haserror = 1;
		}
	}
	if (sym == 11)//int�������������Ӻ�������
	{
		int i = sym;
		sym = getsym();
		if (sym == 1)//��ʶ��
		{
			name[NumOfC] = Word;
			//strcpy_s(name[NumOfC], Word);//�����ʶ��
			sym = getsym();
			if (sym != 39 && sym != 38 && sym != 37)//����Ҫô��;Ҫô��,(��������)Ҫô��(��������)
			{
				errorline[errornum] = linenum;
				error[errornum++] = 22;//ȱ�ٽ�����
				//����һ�����
				while ((c = in.get()) != '\n')
					out << c;
					//putc(c, FOUT);
				linenum++;
				printf("\n%d:\t", linenum);
				haserror = 1;
				sym = getsym();
			}

			if (sym == 38 || sym == 39)//����Ǳ�������,(; ,)
			{
				if (vardefine(11))
					//��������������,�������int����
				{
					//����һ�����
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
			if (sym == 37)//������Ӻ�������,������
			{
				if (returnfunction(11))//��int���ͷ��ص�
				{
					//����һ�����
					while ((c = in.get()) != '\n')
						out << c;
						//putc(c, FOUT);
					linenum++;
					printf("\n%d:\t", linenum);
					haserror = 1;
					sym = getsym();
				}
				if (sym == 14)//main,һ����̱�
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
					error[errornum++] = 15;//��������
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
			error[errornum++] = 27;//ӦΪ��ʶ��
			while ((c = in.get()) != '\n')
				out << c;
				//putc(c, FOUT);
			linenum++;
			printf("\n%d:\t", linenum);
			haserror = 1;
		}
	}
	if (sym == 13)//void,�޷��ر�������
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
			if (sym == 14)//main,һ����̱�
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
				error[errornum++] = 15;//��������
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

//��������˵ ������  
int constdeclaration()
{
	//���������壾    ::=   ����ʶ��������������
	if (!constdefine())//����
	{
		if (sym == 38)//;,һ�����
		{
			if ((sym = getsym()) == 10)//�ٶ���const,���¿�ʼ
			{
				sym = getsym();
				return(constdeclaration());
			}
			else
				return 0;//����,����
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 22;//ȱ��;
			return 1;
		}
	}
	else
		return 1;
}



//������������
int constdefine()
{
	//���������壾          ::=   ����ʶ��������������
	if ((sym = getsym()) == 1)//�Ǳ�ʶ��
	{
		name[NumOfC] = Word;
		//strcpy_s(name[NumOfC], Word);//���ʶ��
		if ((sym = getsym()) == 51)//Ϊ=��
		{
			sym = getsym();
			if (sym == 2)//���ͼ�����
			{
				if (enter(lev, 11, 4))//д���ű�
					return 1;
				if ((sym = getsym()) == 39)//��,��,�ٽ���һ�γ����������
					return(constdefine());
			}
			else
			{
				errorline[errornum] = linenum;
				error[errornum++] = 28;//���������ͺ�ֵ�����Ͳ�ƥ��
				return 1;
			}
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 9;//�˴�Ӧ��'='
			return 1;
		}
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 27;//�˴�ӦΪ��ʶ��
		return 1;
	}
	return 0;//����
}



//��������˵������  
//����������Ѿ���������int a;��int a,�ŵ��õ�
int vardefine(int type)
{
	if (enter(lev, type, 0))//д���ű�,������ʵֻ��int
		return 1;
	if (sym == 39)//����
	{
		if ((sym = getsym()) == 1)//��ʶ��
		{
			name[NumOfC] = Word;
			//strcpy_s(name[NumOfC], Word);//�����ʶ��
			sym = getsym();
			return(vardefine(type));//type������ȥ
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//�˴�ӦΪ��ʶ��
			return 1;
		}
	}
	if (sym == 38)//�ֺ�
	{
		sym = getsym();
		if (sym == 11)//int����,���Ǳ�������,ʶ��
		{
			int i = sym;
			if ((sym = getsym()) == 1)
			{
				name[NumOfC] = Word;
				//strcpy_s(name[NumOfC], Word);//�����ʶ��
				sym = getsym();
				if (sym == 39 || sym == 38)//�ֺŶ���
				{
					if (vardefine(i))
						return 1;
				}
				else if (sym == 37)//������,�Ӻ���,��ȥ��֤
					return 0;
				else
				{
					errorline[errornum] = linenum;
					error[errornum++] = 22;//ȱ�ٽ�����
					return 1;
				}
			}
			else
			{
				errorline[errornum] = linenum;//ӦΪ��ʶ��
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
		error[errornum++] = 22;//ȱ�ٽ�����
		return 1;
	}
	return 0;
}



//�ѱ���������ű�
int enter(int lev, int type, int kind)
{
	if (NumOfC < MAXC - 1)//��ǰԪ��С�ڷ��ű����Ԫ����
	{
		if (charcheck() && kind != 3)//�����ű��Ƿ�������
		{
			errorline[errornum] = linenum;
			error[errornum++] = 8;//�����ظ�����
			return 1;
		}
		Char[NumOfC].lev = lev;//����
		Char[NumOfC].kind = kind;//�ڲ����ͱ���
		Char[NumOfC].type = type;//����ֵ���ͻ��������
		Char[NumOfC].name = name[NumOfC];//��ʶ������

		if (kind == 0 || kind == 3)//���������,��ǰ������ַ��1
		{
			Char[NumOfC].adr = adr;
			adr += 4;
		}

		if (kind == 4)//����
		{
			if (type == 11)
				Char[NumOfC].num = num;
		}
		NumOfC++;
	}
	else
	{
		errorline[errornum] = linenum;//���ű����
		error[errornum++] = 7;
		return 1;
	}
	return 0;
}



//������
int charcheck()
{
	int i;
	for (i = NumOfC - 1; i >= 0; i--)
	{
		if (Char[i].lev == lev)
			if (!strcmp(Char[i].name, name[NumOfC]))//����������
				return 1;
	}
	return 0;
}



//�ڷ��ű��в��ұ�ʶ���Ƿ����,���ڲ���ͻ�ķ���1,�����ڷ���-1
int find()
{
	int i, j = 1;
	for (i = NumOfC - 1; i >= 0; i--)
	{
		if (Char[i].lev == 0 || Char[i].lev == lev)//0��Ķ����Է���,����Ŀ���
			if (!strcmp(Char[i].name, Word))
				return i;
	}
	return -1;
}



//�������
//��������У�    ::=  ����䣾������䣾��
int statementArray()
{
	int a = 0;
	while (sym == 1 || sym == 15 || sym == 18 ||
		sym == 40 || sym == 19 || sym == 20 || sym == 21 || sym == 38)//��ʶ������if����while,scanf,printf,return,�������,�ֺ�
	{
		if ((a = statement()) == 1)
			return 1;
	}
	return 0;
}



//���
//����䣾 ::=  ��������䣾����ѭ����䣾����{��<�������>��}�������Ӻ���������䣾; ������ֵ��䣾; | <�������>;��������䣾;����д��䣾;��;
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

		return(returnstatement());//return����
	case 1://��ʶ��
		if ((a = find()) == -1)
		{
			errorline[errornum] = linenum;
			error[errornum++] = 20;
			return 1;
		}
		sym = getsym();
		if (Char[a].kind == 0 || Char[a].kind == 3)//����������ȶ�����һ����ʶ��,ȷ���ǲ��������
		{
			if (sym == 51)//�����Ⱥ�
				return(assignstatement(Char[a].name));//��ֵ���
			else
			{
				errorline[errornum] = linenum;
				error[errornum++] = 9;
				return 1;
			}
		}
		else if (Char[a].kind == 1 || Char[a].kind == 2)//������һ��������,ȷ�����з���ֵ���޷���ֵ
		{
			if (sym == 37)//������
			{
				if (call(a))
					return 1;
				if (sym == 38)//������
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
	case 40://�������
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


//���ʽ

int expression()
{
	bool anti = false;
	char tempresult[40] = "";
	//�����ʽ��            ::=  �ۣ������ݣ�������ӷ�������������
	if (sym == 31 || sym == 32)//���з��ţ�����
	{
		if (sym == 32)//-��
		{
			anti = true;//Ϊ����
		}
		sym = getsym();
	}

	if (term())//��
	{
		return 1;
	}

	if (anti)
	{
		gen("-", re, "", "");//��
	}

	while (sym == 31 || sym == 32)//
	{
		strcpy_s(tempresult, re);
		int a = sym;
		sym = getsym();
		if (term())//����ֵ��re
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



//��
int term()
//���                ::=   �����ӣ�{���˷�������������ӣ�}
{
	char tempresult[40];
	if (factor())
		return 1;

	while (sym == 33 || sym == 34)//�����
	{
		strcpy_s(tempresult, re);//����ֵ�ȱ���
		int tempsym = sym;
		sym = getsym();
		if (factor())
			return 1;
		//ʶ�������������
		gettempname();//�ٻ��һ��
		if (tempsym == 33)
		{
			gen("MUL", tempresult, re, tempname);// *,t1,t2,t3
		}
		else
		{
			gen("DIV", tempresult, re, tempname);// /,t1,t2,t3
		}
		strcpy_s(re, tempname);//����֮
		/*if(typeofexp2>typeofexp1)
		typeofexp1=typeofexp2;*/
	}
	return 0;
}



//����
//�����ӣ�   ::=  ����ʶ�����������������ʽ�����������������������Ӻ���������䣾
int factor()
{
	//typeofexp1=11;//int
	bool hassamename = false;
	if (sym == 1)//��ʶ��,�������Ӻ����������n
	{
		int a = find();//���ұ�ʾ���Ƿ����
		//�����ȫ�����ҵ�,����ȫ������,����
		//����ڷ�ȫ���ҵ�,���Ǿֲ�����,Ȼ������ȫ��,���û��,����,��,��_
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
			error[errornum++] = 20;//��ʶ��δ����
			return 1;
		}

		if (Char[a].kind == 4)//����,�ӷ��ű����������
		{
			if (Char[a].type == 11)//int�ͳ���,ֱ�Ӷ�ȡ��ֵ
				ss << Char[a].num;
				ss >> re;
				//itoa(Char[a].num, re, 10);//����һ��intֵ
			sym = getsym();
		}
		else if (Char[a].kind == 0 || Char[a].kind == 3)//�������߲���
		{
			strcpy_s(re, Word);//���ر�����
			if (hassamename)
				strcat_s(re, "_");
			sym = getsym();
		}
		else if (Char[a].kind == 2)//�з���ֵ�ĺ���
		{
			if ((sym = getsym()) == 37)//(�����һ����������
			{
				if (!call(a))//����
					return 0;
				else
				{
					errorline[errornum] = linenum;
					error[errornum++] = 17;//ȱ��������
					return 1;
				}
			}
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 11;//�޷���ֵ�����������ڴ˴�
			return 1;
		}
		return 0;
	}
	else if (sym == 37)//������
	{
		sym = getsym();
		if (expression())//�ٴν�����ʽ
			return 1;
		if (sym != 36)//)������
		{
			errorline[errornum] = linenum;
			error[errornum++] = 18;//ȱ��������
			return 1;
		}
		else
			sym = getsym();
		return 0;
	}
	else if (sym == 2)//����
	{
		strcpy_s(re, Word);//�����������
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
		error[errornum++] = 6;//ȱ�����������
		return 1;
	}
	return 0;
}





//�Ӻ�������
int call(int a)
{
	char params[20][20];
	char result[20];
	int i_params = 0;
	int i;
	int count = 0;//��������
	sym = getsym();
	for (i = a + 1; i < MAXC; i++)
	{
		if (Char[i].kind != 3)//����,ö�����еĲ���
			break;
		count++;
		if (expression())//���ص��Ǳ��ʽ��ֵ
			return 1;
		//�������
		strcpy_s(params[i_params++], re);
		if (sym == 39)//,
			sym = getsym();
		else
			break;
	}
	for (int j = 0; j < i_params; j++)
	{
		if (gen("param", params[j], "", ""))
			return 1;//ѹ�����
	}

	//itoa(count, result, 10);
	ss << count;
	ss >> result;
	if (gen("call", Char[a].name, ",", result))//call test,2
		return 1;
	if (Char[i].kind != 3 && sym != 36 || Char[i + 1].kind == 3 && sym == 36)//ǰһ�ִ���������ˣ���һ�ִ�����
	{
		if (count != 0)
		{
			//�޲����
			errorline[errornum] = linenum;
			error[errornum++] = 16;//�������ò�������
			return 1;
		}
	}

	if (sym == 36)//������
	{
		sym = getsym();
		gettempname();
		gen("receive", tempname, "", "");//receive t1;
		strcpy_s(re, tempname);//������ʱ��Ԫ
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




//�з���ֵ�ĺ���
int returnfunction(int type)
{

	adr = 0;
	char currentprocname[40];//��ǰ������
	//�Ѿ�ʶ����������
	Isret = 1;//���޷���ֵ
	if (enter(0, type, 2))//��������д����ű�,������Ϊ0��
		return 1;
	strcpy_s(currentprocname, Char[NumOfC - 1].name);
	//���ɴ���,PROC ������
	gen("proc", currentprocname, "", "");
	lev++;
	sym = getsym();
	//�жϲ���
	while (sym == 11/*||sym==12*/)//��������,int�Ͳ���
	{
		//int i=sym;
		if ((sym = getsym()) == 1)//��������
		{
			strcpy_s(name[NumOfC], Word);
			if (enter(lev, 11, 3))//����д����ű�
				return 1;
			//��ȡһ������
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//ӦΪ��ʶ��
			return 1;
		}
		if ((sym = getsym()) != 39)//���Ƕ���
			break;
		sym = getsym();
		if (sym != 11/*&&sym!=12*/)//int
		{
			errorline[errornum] = linenum;
			error[errornum++] = 4;//ȱ�����ͱ�ʶ��
			return 1;
		}
	}
	if (sym == 36)//������
	{
		if ((sym = getsym()) == 40)//�������,��ʼ���������
		{
			sym = getsym();
			//��������䣾::=�������ۣ�����˵�����֣��ݣۣ�����˵�����֣��ݣ�������У�������
			//����˵��
			if (sym == 10)//const
			{
				//sym=getsym();
				if (constdeclaration())//const����
					return 1;
			}
			//����˵��int a;			
			if (sym == 11/*||sym==12*/)
			{
				int i = sym;
				if ((sym = getsym()) == 1)//��ʶ��
				{
					strcpy_s(name[NumOfC], Word);
					sym = getsym();
					if (sym != 38 && sym != 39)//�����ڷֺŶ���
					{
						errorline[errornum] = linenum;
						error[errornum++] = 22;//ȱ�ٷֺŶ���
						return 1;
					}
					if (sym == 38 || sym == 39)//���ڷֺŻ򶺺�
					{
						if (vardefine(11))
							return 1;
					}
				}
				else
				{
					errorline[errornum] = linenum;//ӦΪ��ʶ��
					error[errornum++] = 27;
					return 1;
				}
			}
			//ǰ�涼�Ǳ�������

			//�������
			if (statementArray())
				return 1;

		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 23;//ȱ���������
			return 1;
		}
		//������������ֶ���һλ
		if (sym == 35)//�Ҵ�����
		{
			sym = getsym();
			//backout();//��������˳����õķ���
			gen("end", currentprocname, "", "");
			if (sym == 11)//int
			{
				int c;
				sym = getsym();
				if (sym == 1)//��ʶ��
				{
					strcpy_s(name[NumOfC], Word);
					if ((sym = getsym()) == 37)//������,��һ����������
						return returnfunction(11);
					else
					{
						errorline[errornum] = linenum;
						error[errornum++] = 17;//ȱ������
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
					error[errornum++] = 27;//ӦΪ��ʶ��
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
			//lev--;//��һ��
			return 0;
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 12;//ȱ���Ҵ�����
			return 1;
		}
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 18;//ȱ��������
		return 1;
	}
}


//�޷���ֵ�ĺ����������
int voidfunction()
{
	char currentprocname[40];
	Isret = 0;
	strcpy_s(name[NumOfC], Word);
	if (enter(0, 0, 1))//д����ű�,������Ϊ0��
		return 1;
	strcpy_s(currentprocname, Word);
	gen("proc", currentprocname, "", "");
	lev++;
	if ((sym = getsym()) != 37)//������
	{
		errorline[errornum] = linenum;
		error[errornum++] = 17;//ȱ������
		return 1;
	}
	sym = getsym();
	while (sym == 11/*||sym==12*/)//����
	{
		int i = sym;
		if ((sym = getsym()) == 1)//��������
		{
			strcpy_s(name[NumOfC], Word);
			if (enter(lev, i, 3))//д��,����һ������
				return 1;
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//�˴�ӦΪ��ʶ��
			return 1;
		}
		if ((sym = getsym()) != 39)//���Ƕ���,�˳�
			break;
		sym = getsym();

		if (sym != 11/*&&sym!=12*/)//������int
		{
			errorline[errornum] = linenum;
			error[errornum++] = 4;//ȱ�����ͱ�ʶ��
			return 1;
		}
	}
	if (sym == 36)//������
	{
		if ((sym = getsym()) == 40)//�������
		{
			sym = getsym();
			if (sym == 10)//const
			{
				//				sym=getsym();
				if (constdeclaration())
					return 1;
			}
			if (sym == 11/*||sym==12*/)//��������
			{
				int i = sym;
				if ((sym = getsym()) == 1)//��ʶ��
				{
					strcpy_s(name[NumOfC], Word);
					sym = getsym();
					if (sym != 38 && sym != 39)//�ֺŶ���
					{
						errorline[errornum] = linenum;
						error[errornum++] = 22;
						return 1;
					}
					if (sym == 38 || sym == 39)//�ֺŶ���
						if (vardefine(i))
							return 1;
				}
				else {
					errorline[errornum] = linenum;
					error[errornum++] = 27;
					return 1;
				}
			}

			if (statementArray())//�������
				return 1;

		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 23;
			return 1;
		}
		if (sym == 35)//�Ҵ����Ÿ���������
		{
			//backout();//�˳�
			//lev--;//��һ��
			//�ٴν���voidfunctionѭ��
			gen("end", currentprocname, "", "");
			if ((sym = getsym()) == 13)//void
			{
				if ((sym = getsym()) == 1)
					return(voidfunction());
			}
			else
				if (sym == 14)//main����
					return 0;
				else
					if (sym == 11)//int���Ӻ�������
					{
						int c;
						sym = getsym();
						if (sym == 1)//��ʶ��
						{
							strcpy_s(name[NumOfC], Word);
							if ((sym = getsym()) == 37)//������,��һ����������
								return returnfunction(11);
							else
							{
								errorline[errornum] = linenum;
								error[errornum++] = 17;//ȱ������
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
							error[errornum++] = 27;//ӦΪ��ʶ��
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
						error[errornum++] = 15;//��������
						return 1;
					}

			return 0;
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 12;//ȱ�Ҵ�����
			return 1;
		}
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 18;//ȱ������
		return 1;
	}
	return 0;
}



//��ֵ���
int assignstatement(char* left)
{
	bool hassamename = false;
	sym = getsym();
	if (expression())
		return 1;
	strcpy_s(Word, left);
	int a = find();//���ұ�ʾ���Ƿ����
	//�����ȫ�����ҵ�,����ȫ������,����
	//����ڷ�ȫ���ҵ�,���Ǿֲ�����,Ȼ������ȫ��,���û��,����,��,��_
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
	if (sym != 38)//ȱ��;
	{
		errorline[errornum] = linenum;
		error[errornum++] = 22;
		return 1;
	}
	sym = getsym();
	return 0;
}



//if�������
//��������䣾          ::=   if������������������������䣾��else����䣾��
int ifstatement()
{
	char templabel[10] = "", beforelabel[10] = "", afterlabel[10] = "", middlelabel[10] = "";
	//���������ʱ���,֮ǰ���,֮����,�м���

	if ((sym = getsym()) == 37)//������
	{
		char symbol[4];//�������
		char tempresult[40];//���һ���ķ��ؽ��
		sym = getsym();
		//�������� ::=  �����ʽ������ϵ������������ʽ���������ʽ��
		if (expression()) //���ʽ,����ֵ��re
			return 1;
		strcpy_s(tempresult, re);
		getnewlabel(templabel);//���һ���µı��,����templabel��

		if (sym == 45 || sym == 46 || sym == 47 || sym == 48 || sym == 49 || sym == 50)//��ϵ�����
		{
			int b = sym;
			sym = getsym();
			if (expression())//���ʽ,����ֵ��re
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

		if (sym != 36)//������
		{
			errorline[errornum] = linenum;
			error[errornum++] = 18;//��������
			return 1;
		}

		gen("goto", templabel, "", "");
		strcpy_s(beforelabel, templabel);
		getnewlabel(templabel);//���һ���µı��,����templabel��
		gen("goto", templabel, "", "");
		strcpy_s(afterlabel, templabel);
		gen(beforelabel, ":", "", "");
		sym = getsym();
		if (statement() == 1)
			return 1;

		if (sym == 16)//else
		{
			getnewlabel(templabel);//���һ���µı��,����templabel��
			gen("goto", templabel, "", "");
			gen(afterlabel, ":", "", "");
			sym = getsym();
			if (statement() == 1)//���
				return 1;
			gen(templabel, ":", "", "");
		}
		else //��else
		{
			gen(afterlabel, ":", "", "");
		}


	}
	else {
		errorline[errornum] = linenum;
		error[errornum++] = 17;//��������
		return 1;
	}
	return 0;
}



//whileѭ�����
//��ѭ����䣾          ::=   while������������������������䣾
int whilestatement()
{
	char templabel[10] = "", beforelabel[10] = "", afterlabel[10] = "", middlelabel[10] = "";
	//���������ʱ���,֮ǰ���,֮����,�м���
	char symbol[20] = "";//�������
	char resulta[20] = "";//���һ���ķ���ֵ
	//int oprnum1=oprnum,oprnum2,oprnum4[MAXCASE],a,j=0;
	if ((sym = getsym()) == 37)//������
	{
		//��������              ::=  �����ʽ������ϵ������������ʽ���������ʽ��
		sym = getsym();
		if (expression()) //���ʽ
			return 1;
		getnewlabel(templabel);//���һ���µı��,����templabel��,l0
		gen(templabel, ":", "", "");
		strcpy_s(beforelabel, templabel);
		getnewlabel(templabel);//���һ���µı��,����templabel��,l1
		strcpy_s(resulta, re);
		if (sym == 45 || sym == 46 || sym == 47 || sym == 48 || sym == 49 || sym == 50)//��ϵ�����
		{
			int b = sym;
			sym = getsym();
			if (expression())//���ʽ,����ֵ��re
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
		getnewlabel(templabel);//���һ���µı��,����templabel��
		gen("goto", templabel, "", "");//goto l2	
		strcat_s(afterlabel, templabel);
		gen(middlelabel, ":", "", "");


		if (sym != 36)
		{
			errorline[errornum] = linenum;
			error[errornum++] = 18;//��������
			return 1;
		}
		sym = getsym();
		if (statement())//���(���������)
			return 1;
		gen("goto", beforelabel, "", "");
		gen(afterlabel, ":", "", "");
	}
	else {
		errorline[errornum] = linenum;
		error[errornum++] = 17;//��������
		return 1;
	}
	return 0;
}




//scanf���
//������䣾            ::=  scanf��(������ʶ����������
int scanfstatement()
{
	int a;
	bool hassamename = false;
	sym = getsym();
	if (sym == 37)//������
	{
		sym = getsym();//��ʶ��
		if (sym != 1)
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//ӦΪ��ʶ��
			return 1;
		}
		a = find();//����ű�
		//�����ȫ�����ҵ�,����ȫ������,����
		//����ڷ�ȫ���ҵ�,���Ǿֲ�����,Ȼ������ȫ��,���û��,����,��,��_
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
			error[errornum++] = 20;//��ʶ��δ�����ʹ����
			return 1;
		}
		if (Char[a].kind == 0 || Char[a].kind == 3)//��������ββ������߱���
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
		if ((sym = getsym()) != 36)//������
		{
			errorline[errornum] = linenum;
			error[errornum++] = 18;//ӦΪ������
			return 1;
		}
		if ((sym = getsym()) == 38)//�ֺ�
		{
			sym = getsym();
			return 0;
		}
		else {
			errorline[errornum] = linenum;
			error[errornum++] = 22;//�ٷֺ�
			return 1;
		}

	}
	errorline[errornum] = linenum;
	error[errornum++] = 17;//ȱ��������
	return 0;
}


//printf���
//��д��䣾   ::=  printf��(��<�ַ���>,�����ʽ ��|<�ַ���>|�����ʽ ��������
int printfstatement()
{

	int	j = 0;
	int	a = 0;

	if ((sym = getsym()) == 37)//������
	{
		sym = getsym();
		if (sym == 5)//�ַ���
		{
			hasprintf = true;
			gen("out", storestring[stringorder - 1].stringname, "", "");//out STR0	
			sym = getsym();//����һ��
			if (sym == 39)//����
			{
				sym = getsym();//����һ��
				if (expression())//��ʶ��
					return 1;
				//printf("sdfs",a);�ṹ
				gen("out", re, "", "");//������ʽ���
				//					sym=getsym();//����һ��

			}
			else if (sym = 36)//������
			{
				//printf("helloworld")���ͽ���			
			}
			else
			{
				errorline[errornum] = linenum;
				error[errornum++] = 18;//ȱ��������
				return 1;
			}
		}
		else if (!expression()) //��ʶ��
		{
			hasprintf = true;
			gen("out", re, "", "");//������ʽ���
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 27;//ȱ�ٷֺ�
			return 1;
		}

		if (sym == 36)//������
		{
			sym = getsym();//����һ��
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 18;//ȱ��������
			return 1;
		}

		if (sym == 38)
		{
			sym = getsym();
		}
		else {
			errorline[errornum] = linenum;
			error[errornum++] = 22;//ȱ�ٷֺ�
			return 1;
		}
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 17;//ӦΪ������
		return 1;
	}
	return 0;

}

//return���
int returnstatement()
{
	if ((sym = getsym()) == 37)//������,�з���ֵ�ĺ���
	{
		if (!Isret)//�޷���ֵ���� 
		{
			errorline[errornum] = linenum;
			error[errornum++] = 14;//�ٷֺ�
			return 1;
		}
		sym = getsym();
		if (expression())//���ʽ
			return 1;
		gen("return", re, "", "");//return c;
		if (sym == 36)//������
		{
			if ((sym = getsym()) == 38)//�ֺ�
			{
				sym = getsym();//�﷨��ȷ
				return 0;
			}
			else {
				errorline[errornum] = linenum;
				error[errornum++] = 22;//�ٷֺ�
				return 1;
			}
		}
		else {
			errorline[errornum] = linenum;//������
			error[errornum++] = 18;
			return 1;
		}
	}
	if (sym == 38)//�ֺ�,�޷���ֵ�ĺ���
	{
		if (Isret)//�з���ֵ����
		{
			errorline[errornum] = linenum;
			error[errornum++] = 29;//�ٷֺ�
			return 1;
		}
		sym = getsym();
		return 0;
	}
	errorline[errornum] = linenum;
	error[errornum++] = 22;//ȱ�ֺ�

	return 1;
}



//main��������
int mainfunction()
{
	Isret = 0;
	start = oprnum;
	strcpy_s(name[NumOfC], Word);
	if (enter(0, 0, 5))//������Ϊ0��
		return 1;
	lev++;
	Char[NumOfC - 1].adr = oprnum;
	if ((sym = getsym()) != 37)//������
	{
		errorline[errornum] = linenum;
		error[errornum++] = 17;//ȱ��������
		return 1;
	}
	sym = getsym();
	//while(sym==11/*||sym==12*/)//int
	//{
	//	int i=sym;
	//	if((sym=getsym())==1)//��ʶ��
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
	if (sym == 36)//������
	{
		if ((sym = getsym()) == 40)//�������
		{
			gen("proc", "main", "", "");//main������ʼ
			sym = getsym();
			if (sym == 10)//const
			{
				//sym=getsym();
				if (constdeclaration())
					return 1;
			}
			if (sym == 11/*||sym==12*/) //��������
			{
				int i = sym;
				if ((sym = getsym()) == 1)//��ʶ��
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
			//�ж����Ĵ�����
			if (sym == 35)
			{
				gen("end", "main", "", "");//main��������
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
		if (sym == 35)//�Ҵ�����
		{
			//backout();
			//lev--;���߱��벻��ջ��,ֱ��һ������һ��lev,��ǰ���ȫ�ֶ���Ϊ0,�����˶����Է���,�����Ĳ�����
			Isret = 0;
			return 0;
		}
		else
		{
			errorline[errornum] = linenum;
			error[errornum++] = 12;//ȱ�Ҵ�����
			return 1;
		}
	}
	else
	{
		errorline[errornum] = linenum;
		error[errornum++] = 18;//ȱ������
		return 1;
	}
	Isret = 0;
	return 0;

}

//����һ�����ʽ
int gen(string op, string src1, string src2, string dst)//op��ָ�����,���������Ƿ��ű����
{
	if (oprnum < MAXOP)//ָ�Ҫ��
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
		error[errornum++] = 19;//ָ���
		return 1;
	}
	return 0;
}

void gettempname()//�����ʱ����
{
	strcpy_s(tempname, "T");
	char temp[3];
	ss << order++;
	ss >> temp;
	//itoa(order++, temp, 10);
	strcat_s(tempname, temp);
	strcpy_s(name[NumOfC], tempname);

	//��ʱ����������ű�
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


void getstringname()//����µı��
{
	char temp[3];
	ss << stringorder;
	ss >> temp;
	//itoa(stringorder, temp, 10);
	strcpy_s(storestring[stringorder].stringname, "STR");
	strcat_s(storestring[stringorder].stringname, temp);
}

int getshellsym()//��shell����ʷ�����,����ֵ��
{
	char  c;//�����ַ�
	int i = 0;//shelltoken�ַ����
	//c = getchar();
	cin >> c;
	while (c == '\n' || c == ' ' || c == 9)//�����޹ص�
	{
		cin >> c;
	}
	while (c != '\n' && c != ' '&& c != 9)//������ֹ�ķ��ž�ֹͣƴ��
	{
		//ƴ��
		shelltoken[i++] = c;
		//c = getchar();
		cin >> c;
	}
	shelltoken[i] = '\0';
	for (i = 0; i <= CommandNum; i++)
	{
		if ((shelltoken==shell[i]) || (strlen(shelltoken) == 1 && shell[i][0] == shelltoken[0]))//��ͬ��ֻ��һ���ַ������ַ���ͬ
		{
			return i;
		}

	}
	return 10;//��Ϊ�ļ���
}

//������
int main()
{
	int c;
	bool hascompileed = false;
	//��shell����ѭ��
	while (1)
	{
		printf("C0Compile>>");
		c = getshellsym();//���һ���ַ�
		switch (c)
		{
		case 0://compile��c
		{c = getshellsym();//����ļ���
		if (c == 10)//�ļ���
		{
			//�ļ�������չ��
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
			printf("\nԴ����:\n");
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
				printf("\n������Ϣ\n");
				int i = errornum;
				for (errornum = 0; errornum < i; errornum++)
				{
					error_msg(error[errornum]);
				}
				break;
			}
			if (errornum == 0)
			{
				printf("\n\n����ͨ��!\n\n");
				hascompileed = true;
				fclose(FIN);
			}
			break;
		}}
		case 1://link��l
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
		case 2://run��r
		{	c = getshellsym();
			char temp[40];
			strcpy_s(temp, shelltoken);
			strcat_s(temp, ".exe");
			system(temp);
			break;
		}
		case 3://quit��q
		{
			if (FIN != NULL)
			fclose(FIN);
			exit(0);
		}
		case 4://help��h
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
			if (!hascompileed)//û����
			{
				printf("You shoud first compile it.");
				continue;
			}
			//��ȡһ���ļ���
			c = getshellsym();//����ļ���
			if (c == 10)//�ļ���
			{
				//�ļ�������չ��
				if (strchr(shelltoken, '.') == NULL)
					strcat_s(shelltoken, ".txt");
				out.open(shelltoken);
				out.open(shelltoken, ios::in||ios::out);
				//FOUT = fopen(shelltoken, "w");
				int i;
				for (i = 0; i < oprnum; i++)
				{
					//����к�,��������,����������
					fprintf(FOUT, "%s\t%s\t%s\t%s\n", opra[i].op,
						opra[i].src1, opra[i].src2, opra[i].dst);
				}
				//�м����д��ɹ�
				printf("Code has been write into file successfully.\n");
				fclose(FOUT);
				break;
			}
		}
		//case 6://asm
		/*{
			if (!hascompileed)//û���ļ�
			{
				printf("You shoud first compile it.");
				continue;
			}

			//��ȡһ���ļ���
			c = getshellsym();//����ļ���
			if (c == 10)//�ļ���
			{
				//�ļ�������չ��
				if (strchr(shelltoken, '.') == NULL)
					strcat(shelltoken, ".asm");
				FOUT = fopen(shelltoken, "w");
				toasm();//���ɻ��
				int i;
				for (i = 0; i < asmorder; i++)
				{
					//����к�,��������,����������
					fprintf(FOUT, "%s\n", asmpr[i]);
				}
				//�м����д��ɹ�
				fclose(FOUT);
				printf("Assembly code has been write into file successfully.\n");
				break;
			}
		}*/
		case 6://print��p
		{
			c = getshellsym();//����ļ���
			if (c == 10)
			{
				//FILE *fin;
				char *one_line; // �����һ�� 
				int buff_size = 120; //��������ַ�����С 
				in.open(shelltoken);
				if (!in) {
					printf("can not open file %s\n", shelltoken);
					break;
				};

				one_line = (char *)malloc(buff_size * sizeof(char));

				while (in.eof()) {
					//printf("%s", one_line); // ��һ�д�һ�� 
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