```
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class MySQLTest {
	private static final String URL = "jdbc:mysql://localhost:3306/lesson?useUnicode=true&"
			+ "characterEncoding=utf8&serverTimezone=UTC";/*jdbc:mysql://数据库ＩＰ/数据库名*/
    private static final String USER = "root";//数据库用户名
    private static final String PASSWORD = "123456";//对应的用户密码
    
	
	public static void main(String[] args) {
		Connection conn = null;
        Statement stmt = null;
//      String JDBC_DRIVER = "com.mysql.jdbc.Driver";  
//		Class.forName(JDBC_DRIVER);//This is deprecated, The new driver  is automatically registered
	
        try{
            conn = DriverManager.getConnection(URL,USER,PASSWORD);
            stmt = conn.createStatement();
            String sql;
            sql = "select * from student";
            ResultSet rs = stmt.executeQuery(sql);
            System.out.println("id\tname");
            while(rs.next()){
                int id = rs.getInt("sID");
                String name = rs.getString("sName");
                System.out.println(id + "\t"+name);
            }
            rs.close();
            stmt.close();
            conn.close();
        }catch(SQLException se){
            se.printStackTrace();
        }finally{
            try{
                if(stmt!=null) stmt.close();
            }catch(SQLException se2){
            	se2.printStackTrace();
            }
            try{
                if(conn!=null) conn.close();
            }catch(SQLException se){
                se.printStackTrace();
            }
        }
    }
}
```
