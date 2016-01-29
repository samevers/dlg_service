/************************************************************
  Copyright (C), 1998-2006, Tencent Technology Commpany Limited

  文件名称: twsetypedef.h
  作者: swordshao       
  日期: 2006.12.12
  版本: 1.0
  模块描述: 平台无关的数据类型定义
            每个数据类型的定义以T开头，T表示类型(Type)的意思。

  修改历史:         
      <author>    <time>   <version >   <desc>    
     swordshao  2006.12.12    1.0        创建
*************************************************************/

#ifndef __TWSE_TYPE_DEF_H__
#define __TWSE_TYPE_DEF_H__


typedef char                        TCHAR;
typedef unsigned char               TUCHAR;
typedef char                        TINT8;
typedef unsigned char               TUINT8;
typedef short                       TINT16;
typedef unsigned short              TUINT16;
typedef int                         TINT32;
typedef unsigned int                TUINT32;
typedef float                       TFLOAT32;
typedef double                      TFLOAT64;
typedef void                        TVOID;
typedef int                         TBOOL; 


#ifdef WIN32
    typedef __int64                 TINT64;
    typedef unsigned __int64        TUINT64;
#else
    #include <sys/types.h>
    typedef __int64_t               TINT64;
    typedef __uint64_t              TUINT64;
#endif


/** 3字节无符号整数 */
#pragma pack(1)
typedef struct _TUINT24
{
    TUINT8  h;
    TUINT16 l;
    TUINT32 GetVal(){return ((this->h << 16) | this->l);};
    /*********************** = ****************************/

	operator TUINT32()
	{
		return GetVal();
	}

    struct _TUINT24 operator=(TUINT8 a[3])
    {
        this->h = a[0];
        this->l = *(TUINT16*)(a+1);
        return *this;
    };

    struct _TUINT24 operator=(struct _TUINT24 a)
    {
        this->h = a.h;
        this->l = a.l;
        return *this;
    };

    struct _TUINT24 operator=(TUINT32 &a)
    {
        this->l = (TUINT16)a;
        this->h = (TUINT8)(a >> 16);
        return *this;
    };

    struct _TUINT24 operator=(TUINT16 &a)
    {
        this->l = a;
        this->h = 0;
        return *this;
    };

    /********************** > >= ***************************/
    bool operator>(struct _TUINT24 b)
    {
        if(this->h > b.h) return true;
        if(this->h < b.h)	return false;
        return this->l > b.l;
    };
    bool operator>=(struct _TUINT24 b)
    {
        if(this->h > b.h) return true;
        if(this->h < b.h)	return false;
        return this->l >= b.l;
    };
    bool operator>(TUINT32 b)
    {
        if(this->h > (b>>16)) return true;
        if(this->h < (b>>16))	return false;
        return this->l > (b&0xffff);
    };
    bool operator>=(TUINT32 b)
    {
        if(this->h > (b>>16)) return true;
        if(this->h < (b>>16))	return false;
        return this->l >= (b&0xffff);
    };
    /****************** < <= ******************************/
    bool operator<(struct _TUINT24 b)
    {
        if(this->h < b.h) return true;
        if(this->h > b.h)	return false;
        return this->l < b.l;
    };
    bool operator<=(struct _TUINT24 b)
    {
        if(this->h < b.h) return true;
        if(this->h > b.h)	return false;
        return this->l <= b.l;
    };
    bool operator<(TINT32 b)
    {
        if(this->h < (b>>16)) return true;
        if(this->h > (b>>16))	return false;
        return this->l < (b&0xffff);
    };
    bool operator<=(TINT32 b)
    {
        if(this->h < (b>>16)) return true;
        if(this->h > (b>>16))	return false;
        return this->l <= (b&0xffff);
    };
    /***************** == *********************************/
    bool operator==(struct _TUINT24 b)
    {
        return (b.h == this->h && b.l == this->l);
    };
    bool operator==(TUINT32 b)
    {
        return ((b >> 16) == this->h && (b & 0xffff) == this->l);
    };
    bool operator==(TUINT16 b)
    {
        return (0 == this->h && b == this->l);
    };
    /*************** != *********************************/
    bool operator!=(struct _TUINT24 b)
    {
        return (b.h != this->h || b.l != this->l);
    };
    bool operator!=(TUINT32 b)
    {
        return ((b >> 16) != this->h || (b & 0xffff) != this->l);
    };
    bool operator!=(TUINT16 b)
    {
        return (0 != this->h || b != this->l);
    };
} TUINT24;
#pragma  pack()
#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL
#define NULL	0
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAIL
#define FAIL -1
#endif


#endif //__TWSE_TYPE_DEF_H__

