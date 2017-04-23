import java.util.concurrent.Semaphore;

/**
 * @author cristian.chilipirea
 *
 */
public class Main {
	static int N_ITERATIONS = 100;
	static int MAX_EL = 10000;

	public static void main(String[] args) {
		
		
		Semaphore[] s = new Semaphore[MAX_EL];
		
		for(int i = 0 ; i < MAX_EL; ++i)
			s[i] = new Semaphore(1);
		
		Thread threads[] = new Thread[3];
		
		for (int j = 0; j < N_ITERATIONS; j++) {
			Semaphore s1 = new Semaphore(-3 + 2);
			TreeNode tree = new TreeNode(1);
			threads[0] = new Thread(new ReadTreePart(tree, "treePart1.txt", s1, s));
			threads[1] = new Thread(new ReadTreePart(tree, "treePart2.txt", s1, s));
			threads[2] = new Thread(new VerifyTree(tree, s1));
			for (int i = 0; i < 3; i++)
				threads[i].start();
			for (int i = 0; i < 3; i++) {
				try {
					threads[i].join();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
	}
}