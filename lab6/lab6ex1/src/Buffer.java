import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.Semaphore;

/**
 * @author cristian.chilipirea
 *
 */
public class Buffer {
int a;
	
	//acquire <=> P
	//release <=> V
	ArrayBlockingQueue<Integer> q = new ArrayBlockingQueue<Integer>(100);
	void put(int value) {
		
		try {
			q.put(value);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}

	int get() {
		Integer x = 0;
		try {
			x = q.take();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return x;
	}
}
