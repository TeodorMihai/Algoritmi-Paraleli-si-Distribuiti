import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.concurrent.Semaphore;

/**
 * @author cristian.chilipirea
 *
 */
public class ReadTreePart implements Runnable {
	
	TreeNode tree;
	String fileName;
	Scanner sc;
	Semaphore s1;
	Semaphore[] s;

	public ReadTreePart(TreeNode tree, String fileName, Semaphore _s1, Semaphore[] _s) {
		this.tree = tree;
		this.fileName = fileName;
		try {
			sc = new Scanner(new File(fileName));
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		s1 = _s1;
		s = _s;
		
	}

	@Override
	public void run() {
		
		while(sc.hasNext()) {
			
			Integer father;
			Integer son;
			son = sc.nextInt();
			father = sc.nextInt();
			
			TreeNode t = null;
			
			while(t == null) {
		
				t = tree.getNode(father);
				
				if(t != null) 
					synchronized (t) { 
						t.addChild(new TreeNode(son));
					}
			}
			
			if(sc.hasNext() == false)
				s1.release();
		}
	}
}
