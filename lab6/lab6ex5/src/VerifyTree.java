import java.util.concurrent.Semaphore;

/**
 * @author cristian.chilipirea
 *
 */
public class VerifyTree implements Runnable {
	TreeNode tree;
	Semaphore s1;

	public VerifyTree(TreeNode tree, Semaphore _s1) {
		this.tree = tree;
		s1 = _s1;
	}

	boolean isCorrect(TreeNode tree) {
		if (tree == null)
			return true;
		if (tree.name >= 64)
			return true;
		if (tree.left == null)
			return false;
		if (tree.right == null)
			return false;
		boolean aux = ((tree.left.name + tree.right.name) == (tree.name * 4 + 1));
		return aux && isCorrect(tree.left) && isCorrect(tree.right);
	}

	@Override
	public void run() {
		
		try {
			s1.acquire();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if (isCorrect(tree))
			System.out.println("Correct");
		else
			System.out.println("Wrong");
		
		s1.release();

	}
}
