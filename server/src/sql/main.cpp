#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/metadata.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/exception.h>
#include <cppconn/warning.h>

#define DBHOST "tcp://127.0.0.1:3306"
#define USER "zyb"
#define PASSWORD "zyb151"
#define DATABASE "chess_server"

using namespace std;
using namespace sql;


int main()
{
     try
    {
        //连接数据库
        Driver *driver = get_driver_instance();
        Connection *conn = driver->connect(DBHOST, USER, PASSWORD);

        Statement *stm;


        if(!conn->isValid()){
            cout<<"数据库连接无效"<<endl;
            return 0;
        }else
            cout<<"数据库连接成功"<<endl;
        //创建 test 表，添加数据
        stm = conn->createStatement();
        bool ret = stm->execute("use " DATABASE);
        std::cout << ret << std::endl;
        stm->execute("DROP TABLE IF EXISTS test");
        stm->execute("CREATE TABLE test(id INT,lable CHAR(1))");
        stm->execute("INSERT INTO test(id,lable) VALUES(6,'A')");
        stm->execute("INSERT INTO test(id,lable) VALUES(3,'A')");
        stm->execute("INSERT INTO test(id,lable) VALUES(2,'A')");
        stm->execute("INSERT INTO user VALUES(5,'zyb1', 'hhhh', 10, 9)");

        //升序查询
        ResultSet *rss;
        rss = stm->executeQuery("SELECT id,lable FROM test ORDER BY id ASC");
        while (rss->next())
        {
            /* code */

            int id = rss->getInt(1);
            string lable = rss->getString("lable");

            cout << "id:" << id << ","
                 << "lable:" << lable << endl;
        }


        //删除
        stm->execute("DELETE FROM test WHERE id=3");

        //改
        stm->execute("UPDATE test SET lable='B' WHERE id=2");

        delete stm;
        delete conn;
        delete rss;
    
    }
    catch (const SQLException &sqle)
    {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "<< __LINE__ << endl;
        std::cerr << "sql errcode:" << sqle.getErrorCode() << ",state:" << sqle.getSQLState() << ",what:" << sqle.what() << endl;
    }

    return 0;
}
