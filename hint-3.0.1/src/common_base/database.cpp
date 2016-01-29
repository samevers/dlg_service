//-----------------------------------------------------------------------------
//  database.cpp
//-----------------------------------------------------------------------------
#include "database.h"
#include "time.h"
#include "sys/time.h"
#include "unistd.h"
#include <stdio.h>

	CMysqlExecute::CMysqlExecute( CMysql* pMysql )
: m_pMySql(pMysql), m_pTable(0)
{
}

CMysqlExecute::~CMysqlExecute()
{
	unlock();
}

bool CMysqlExecute::lock( const char* pTable, bool bWrite )
{
	char locksql[512];

	if( bWrite )
		sprintf(locksql, "lock table %s write", pTable);
	else
		sprintf(locksql, "lock table %s read", pTable);

	if( execute(locksql) < 0 )
		return false;

	m_pTable = pTable;
	return true;
}

bool CMysqlExecute::unlock()
{
	if( !m_pTable )
		return true;

	char unlocksql[512];

	sprintf(unlocksql, "unlock table %s write", m_pTable);

	m_pTable = 0;
	return ( execute(unlocksql) >= 0 );
}

int CMysqlExecute::execute( const char* pStr )
{
	bool isSuccess = true;
	timeval t1, t2;
	gettimeofday(&t1, NULL);
	if(mysql_query(m_pMySql->getHandle(), pStr)) {
		isSuccess = false;
		if(m_pMySql->errorno() == CR_SERVER_GONE_ERROR) {
			if(m_pMySql->reconnect()) {
				if(mysql_query(m_pMySql->getHandle(), pStr) == 0)
					isSuccess = true;
			}
		}
	}

	gettimeofday(&t2, NULL);
	if(isSuccess) {
		fprintf(stderr, "[Mysql::execute]time:%ld\n",(t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec);
		return mysql_affected_rows(m_pMySql->getHandle());
	}
	else {
		fprintf(stderr, "[Mysql::execute]time:%ld\n",(t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec);
		return -1;
	}
}

long long CMysqlExecute::execute2( const char* pStr )
{
	bool isSuccess = true;
	timeval t1,t2;
	gettimeofday(&t1,NULL);
	if( mysql_query(m_pMySql->getHandle(), pStr) )
	{
		isSuccess = false;
		if(m_pMySql->errorno() == CR_SERVER_GONE_ERROR) {
			if(m_pMySql->reconnect()) {
				if(mysql_query(m_pMySql->getHandle(), pStr) == 0)
					isSuccess = true;
			}
		}
	}

	gettimeofday(&t2, NULL);
	if(isSuccess) {
		fprintf(stderr, "[Mysql::execute]time:%ld\n",(t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec);
		return mysql_insert_id(m_pMySql->getHandle());
	}
	else {
		fprintf(stderr, "[Mysql::execute]time:%ld\n",(t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec);
		return -1;
	}
}

	CMysqlRow::CMysqlRow( CMysqlQuery* pQuery )
: m_pQuery(pQuery),
	m_row(0),
	m_lengths(0), 
	m_fields(0)
{
	m_types[0] = 0;
}

char * CMysqlRow::fieldValue(const std::string &fieldName)
{
	std::map<std::string, unsigned int>::iterator itr;
	itr = m_pQuery->m_field_index.find(fieldName);
	if(itr != m_pQuery->m_field_index.end())
	{
		return fieldValue(itr->second);
	}

	fprintf(stderr, "[CMysqlRow::fieldValue]field name(%s) error\n", fieldName.c_str());
	return NULL;
}

	CMysqlQuery::CMysqlQuery( CMysql* pMysql )
: CMysqlExecute(pMysql),
	m_pResult(0),
	m_row(this),
	m_bEnd(false),
	m_rows(0)
{
}

CMysqlQuery::~CMysqlQuery()
{
	clear();
}

void CMysqlQuery::clear()
{
	if( m_pResult )
		mysql_free_result(m_pResult);
	m_pResult = 0;
	m_row.m_row = 0;
	m_row.m_lengths = 0;
	m_row.m_fields = 0;
	m_bEnd = false;
	m_rows = 0;
	m_field_index.clear();
}

int CMysqlQuery::execute( const char* pStr )
{
	clear();

	bool isSuccess = true;
	if( mysql_query(m_pMySql->getHandle(), pStr) )
	{
		isSuccess = false;
		if(m_pMySql->errorno() == CR_SERVER_GONE_ERROR) {
			if(m_pMySql->reconnect()) {
				if(mysql_query(m_pMySql->getHandle(), pStr) == 0)
					isSuccess = true;
			}
		}
	}

	if(isSuccess) {
		m_pResult = mysql_store_result(m_pMySql->getHandle());
		if( m_pResult )  // there are rows
		{
			m_rows = mysql_num_rows(m_pResult);
			return 0;
		}
	}
	else
		return -1;
}

CMysqlRow* CMysqlQuery::fetch()
{
	if( m_bEnd )
		return 0;

	if( !m_row.m_row )
	{
		m_row.m_row = mysql_fetch_row(m_pResult);
		m_row.m_lengths = mysql_fetch_lengths(m_pResult);
		m_row.m_fields = mysql_num_fields(m_pResult);
		mysql_field_seek(m_pResult, 0);
		unsigned int i;
		for( i=0; i<m_row.m_fields; i++ )
		{
			MYSQL_FIELD* field = mysql_fetch_field(m_pResult);
			m_row.m_types[i] = field->type;
			m_field_index.insert(std::pair<std::string, unsigned int>(field->name, i));
		}
	}
	else
	{
		m_row.m_row = mysql_fetch_row(m_pResult);
		//m_row.m_lengths = mysql_fetch_lengths(m_pResult);
	}
	if( !m_row.m_row )
	{
		clear();
		m_bEnd = true;
		return 0;
	}

	return &m_row;
}

CMysql::CMysql(const std::string &dbIP, const std::string &userName, const std::string &password, const std::string &dbName, const std::string &initSQL) : m_bClosed(true), dbIP_(dbIP), userName_(userName), password_(password), dbName_(dbName), initSQL_(initSQL)
{
	init();
}

CMysql::~CMysql()
{
	close();
}

const char* CMysql::error()
{
	return mysql_error(&m_handle);
}

unsigned int CMysql::errorno()
{
	return mysql_errno(&m_handle);
}

bool CMysql::connect()
{
	pthread_mutex_lock(&initMutex);
	bool ret = !!mysql_real_connect(&m_handle, dbIP_.c_str(), userName_.c_str(), password_.c_str(), 0, 0, 0, 0);
	pthread_mutex_unlock(&initMutex);

	return ret;
}

bool CMysql::reconnect()
{
	fprintf(stderr, "[CMysql::reconnect] try to reconnect to mysql\n");
	bool isSuccess = false;
	close();
	init();
	if(isClosed()) {
		fprintf(stderr, "[CMysql::reconnect] Mysql init fail\n");
	} else if(!connect()) {
		fprintf(stderr, "[CMysql::reconnect] connect to %s fail, error info:%s\n", dbIP_.c_str(), error());
	} else if(!open()) {
		fprintf(stderr, "[CMysql::reconnect] open database %s fail, error info:%s\n", dbName_.c_str(), error());
	} else if(mysql_query(&m_handle, initSQL_.c_str())) {
		fprintf(stderr, "[CMysql::reconnect] set characterset fail:%s, error info:%s\n", initSQL_.c_str(), error());
	} else {
		isSuccess = true;
	}
	return isSuccess;
}

bool CMysql::open()
{
	return !mysql_select_db(&m_handle, dbName_.c_str());
}

void CMysql::init()
{
	if( m_bClosed )
		m_bClosed = !mysql_init(&m_handle);
}

void CMysql::close()
{
	if( !m_bClosed )
	{
		mysql_close(&m_handle);
		m_bClosed = true;
	}
}

unsigned long CMysql::escapeString(const std::string &in, std::string &out)
{
	char *out_buf = new char[in.size()*2+1];
	unsigned long len;
	len = mysql_real_escape_string(&m_handle, out_buf, in.c_str(), in.size());
	out.assign(out_buf, len);
	delete [] out_buf;
	return len;
}

unsigned long CMysql::escapeString(std::string &str)
{
	char *out_buf = new char[str.size()*2+1];
	unsigned long len;
	len = mysql_real_escape_string(&m_handle, out_buf, str.c_str(), str.size());
	str.assign(out_buf, len);
	delete [] out_buf;
	return len;
}

pthread_mutex_t CMysql::initMutex = PTHREAD_MUTEX_INITIALIZER;
