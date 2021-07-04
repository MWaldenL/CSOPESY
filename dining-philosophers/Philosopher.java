import java.util.Random;
import java.util.concurrent.*;

public class Philosopher extends Thread {
  static final int THINKING = 0, EATING = 1, HUNGRY = 2;
  int id, state;
  String name;
  Semaphore semaphore;

  public Philosopher(int id, Semaphore s) {
    super("P" + id);
    this.name = "P" + id; 
    this.id = id;
    this.semaphore = s;
  }

  public void pickup(int i) {
    try {
      S.state[i] = HUNGRY;
      semaphore.acquire(); // wait
      test(i);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
  } 
  
  public void putdown(int i) {
    System.out.printf("Philosopher %d released its left and right chopsticks.\n", i);
    S.state[i] = THINKING;
    test((i+4) % 5);
    test((i+1) % 5);
  }

  public void test(int i) {
    if (S.state[(i+1) % 5] != EATING && S.state[(i+4) % 5] != EATING && S.state[i] == HUNGRY) {
      System.out.printf("Philosopher %d acquired its left and right chopsticks.\n", i);
      S.state[i] = EATING;
      this.semaphore.release();
    }
  }

  @Override 
  public void run() {
    while (true) {
      if (this.getName().equals(this.name)) { // this thread is running
        switch (S.state[this.id])  {
          case THINKING:
            try {
              System.out.printf("Philosopher %d is thinking.\n", this.id);
              Thread.sleep(new Random().nextInt(501));
              this.pickup(this.id);
              Thread.sleep(new Random().nextInt(501));
            } catch (InterruptedException e) {}
            break;
          case EATING: 
            try {
              System.out.printf("Philosopher %d is eating.\n", this.id);
              Thread.sleep(new Random().nextInt(501));
              this.putdown(this.id);
            } catch (InterruptedException e) {}
            break;
        }
      }
    }
  }
}