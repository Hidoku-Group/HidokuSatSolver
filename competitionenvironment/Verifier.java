//This program is to convert a formatted Hidoku to another format.  
//
//hidoku with size 5
//--------------------------
//|    |    |    |    |    |
//| 1  |    |    |    |    |
//|    |    |    |    |    |
//--------------------------
//|    |    |    |    |    |
//|    |    | 13 | 17 |    |
//|    |    |    |    |    |
//--------------------------
//|    |    |    |    |    |
//|    | 25 | 10 |    | 18 |
//|    |    |    |    |    |
//--------------------------
//|    |    |    |    |    |
//|    |    |    |    | 7  |
//|    |    |    |    |    |
//--------------------------
//|    |    |    |    |    |
//|    |    |    |    |    |
//|    |    |    |    |    |
//--------------------------
// Reading above file and convert into cnf file
 
import java.io.*;
import java.util.*;

import javax.swing.JOptionPane;
public class Verifier {
	static int maxVars;
	static int maxCls;
	static StringBuffer output;

	public static void main(String[] args)throws IOException{
	
		Boolean runInEclipse = false; 
		String file1 = "";
		String file2 = "";
		int files = 0;
		
		if( runInEclipse ) {
			
			String str = JOptionPane.showInputDialog(null, "Enter number of files: ", 
					"n", 1);
			
			files = Integer.parseInt(str);
			
			if( files < 1 || files > 2 ) { 
				System.err.println( "specify 1 or 2!" );
				System.exit(-1);
			}
			
			for( int i = 0 ; i < files ; ++ i ) {
				if( i == 0) {
					file1 = JOptionPane.showInputDialog(null, "Enter file " + Integer.toString(i+1) + "/home/norbert/projects/hidoku/solutions/hidoku" ,"Filename", 1);
					file1 = "/home/norbert/projects/hidoku/solutions/hidoku" + file1;
				}
				if( i == 1) {
					file2 = JOptionPane.showInputDialog(null, "Enter file " + Integer.toString(i+1) + "/home/norbert/projects/hidoku/hidokus/hidoku" ,"Filename", 1);
					file2 = "/home/norbert/projects/hidoku/hidokus/hidoku" + file2;	
				}
			}
			
		} else {
		    if( args.length < 2 ) {
		    	System.err.println( "no single file specified!" );
		    	System.exit(-1);
		    }
		    System.err.println("args: " + args.length);
		    if( args.length > 0 ){ file1 = args[0]; files = 1; }
		    if( args.length > 1 ){ file2 = args[1]; files = 2; }
		    
		    System.err.println("file1: " + file1 + " file2: " + file2);
		}
		
	    System.err.println( "read file " + file1 );
	    int field1 [][] = readFile(file1);
	    System.err.println("length of field: " + Integer.toString( field1.length ) );
	    int n = field1.length;
	    int field2 [][] = null;
	    
	    	System.err.println("verify solution of the first file" );	
	    	
	    	int cx = -1, cy = -1;
	    	
	    	for( int y = 0 ; y < n ; ++ y)
			{
				for( int x = 0; x < n; ++ x )
				{
					if( field1[y][x] == 1) {
						cx=x;cy=y;
					}
				}
			}

	    	
	    	if( cx == -1 ) { System.err.println("Have not found number 1 in the field"); System.exit(-1); }
	    	int nr = 1;
	    	System.err.println("Found number 1 in field (" +cx+","+cy+")");
	    	
	    	for( int i = 2; i <= n*n; ++ i )
	    	{
	    		int nx = cx, ny = cy;
	    		// check all neighbors
	    		for( int dx = -1; dx < 2; dx ++ ) {
	    			if( cx + dx < 0 ) continue;
	    			if( cx + dx >= n ) continue;
	    			for( int dy = -1; dy < 2; dy ++ ) {
		    			if( cy + dy < 0 ) continue;
		    			if( cy + dy >= n ) continue;
		    			if( dx == 0 && dy == 0) continue;
		    			// found successor in neighbor fields
		    			if( field1[cy+dy][cx+dx] == nr + 1) {
		    				nx = cx+dx; ny = cy+dy;
		    			}
		    			
		    		}	
	    		}
	    		if( nx == cx && ny == cy ) {
	    			System.err.println("Have not found successor of " + nr + " around field (" +cx+","+cy+")");
	    			System.exit(-1);
	    		}
	    		nr = nr + 1;
	    		cx = nx; cy = ny;
	    	}
	    	
	    	
	    	if( nr != n*n ) {
	    		System.err.println("Last number " + nr + " in field (" +cx+","+cy+") is not " + n*n );
    			System.exit(-1);
	    	}
	    	
	    	System.err.println("first file is a valid hidoku" );
	    	
	    if( files == 2)
	    {
	    	System.err.println( "read file " + file2 );
		    field2 = readFile(file2);
	    	System.err.println("cross check file with initial hidoku (file2)" );
	    	
	    	for( int y = 0 ; y < n ; ++ y)
			{
				for( int x = 0; x < n; ++ x )
				{
					if( field2[y][x] == 0 ) continue;
					if( field1[y][x] != field2[y][x]) {
						System.err.println( "Match error: field1 (" + x + "," + y + ")=" + field1[y][x] + "; field2 (" + x + "," + y + ")=" + field2[y][x] );
		    			System.exit(-1);		
					}
				}
			}
	    }
	    System.err.println("passed all checks.");
	    System.exit(0);
	}
	
	public static int [][] readFile(String filename) throws IOException {
		// Open the file
		FileInputStream fstream = new FileInputStream(filename);
		// Get the object of DataInputStream
		DataInputStream in = new DataInputStream(fstream);
		BufferedReader br = new BufferedReader(new InputStreamReader(in));


		String strLine; // read a line
		String firstL = new String("hidoku with size ");
		// reading the first row
		String firstLine = br.readLine();
		
		
		
		String numberStr = firstLine.substring(firstL.length());
		int number;
		// exploiting the filename to get n.

		int n = Integer.parseInt(numberStr);
		
		maxVars=n*n*n*n;
		
		int field [][] = new int [n][n];
		
		maxCls=0;
		output=new StringBuffer("");
		
		for (int i = 1; i <= n; i++) {
			br.readLine();// --- line
			br.readLine();// | line
			strLine = br.readLine();// numbers line

			int first = 0;
			int second = 0;
			for (int j = 1; j <= n; j++) {
				first = strLine.indexOf("|", second);
				second = strLine.indexOf("|", first + 1);
				numberStr = strLine.substring(first + 1, second - 1);
				numberStr = numberStr.trim();
				if (!numberStr.equals("")){
					//processing here
					number= Integer.parseInt(numberStr);
					field[i-1][j-1] = number;
				}
			}

			br.readLine();// | line
		}
		
		
		// display parsed hidoku
		
		System.err.println( "Hidoku in matrix format: " );
		for( int y = 0 ; y < n ; ++ y)
		{
			for( int x = 0; x < n; ++ x )
			{
				System.err.print( field[y][x] );
				System.err.print( " " );
			}
			System.err.println();
		}
		
		return field;
	}

}
