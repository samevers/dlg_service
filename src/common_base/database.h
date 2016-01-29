#ifndef __DATABASE_H__
#define __DATABASE_H__

//-----------------------------------------------------------------------------
/// mysql
//-----------------------------------------------------------------------------
//#include <mysql/my_global.h>
#include <mysql/mysql.h>
#include <mysql/errmsg.h>
#include <map>
#include <string>

class CMysql;

/*执行mysql命令的类*/
class CMysqlExecute
{
	public:
		CMysqlExecute( CMysql* pMysql );
		virtual ~CMysqlExecute();

		bool lock( const char* pTable, bool bWrite );
		bool unlock();

		virtual int execute( const char* pStr );
		//返回上次插入时自动生成的id
		virtual long long execute2( const char* pStr );

	protected:
		CMysql* m_pMySql;
		const char* m_pTable;
};

class CMysqlQuery;

/*查询结果的类*/
class CMysqlRow 
{
	public:
		CMysqlRow( CMysqlQuery* pQuery );
		virtual ~CMysqlRow() {}

		char * fieldValue( int nIndex ) { return m_row[nIndex]; }
		char * fieldValue(const std::string &fieldName);

		int fieldType( int nIndex ) { return m_types[nIndex]; }
		int fieldLength( int nIndex ) { return (int)m_lengths[nIndex]; }
		int fieldNum() { return (int)m_fields; }        

	protected:
		CMysqlQuery* m_pQuery;
		MYSQL_ROW m_row;
		unsigned long* m_lengths;
		unsigned int m_fields;          
		int m_types[100];

		friend class CMysqlQuery;
};

/*执行mysql查询（select）命令的类*/
class CMysqlQuery : public CMysqlExecute
{
	public:
		CMysqlQuery( CMysql* pMysql );
		virtual ~CMysqlQuery();

		int execute( const char* pStr );
		CMysqlRow* fetch();

		bool isEnd() { return m_bEnd; }
		int rows() { return (int)m_rows; }

	protected:
		void clear();

		MYSQL_RES *m_pResult;
		CMysqlRow m_row;
		bool m_bEnd;
		unsigned int m_rows;

	private:
		//field name to data array index
		std::map<std::string, unsigned int> m_field_index;

		friend class CMysqlRow;
};


/*连接打开mysql的类*/
class CMysql
{
	public:
		CMysql(const std::string &dbIP, const std::string &userName, const std::string &password, const std::string &dbName, const std::string &initSQL="set names gbk");
		virtual ~CMysql();

		bool connect();
		bool reconnect();
		bool open();
		void init();
		void close();
		bool isClosed() { return m_bClosed; }

		MYSQL* getHandle() { return &m_handle; }
		unsigned long escapeString(const std::string &in, std::string &out);
		unsigned long escapeString(std::string &str);

		const char* error();
		unsigned int errorno();

	private:
		CMysql();

	protected:            
		MYSQL m_handle;
		bool m_bClosed;

		std::string dbIP_;
		std::string userName_;
		std::string password_;
		std::string dbName_;
		std::string initSQL_; //like "set names gbk"
	
	private:
		static pthread_mutex_t initMutex;
};

#endif // __DATABASE_H__

