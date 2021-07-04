/*
***************************************************************
Names:    Badulis, Keith Gabriel
          Lua, Matthew Walden
Group:    37
Section:  S15
***************************************************************
*/

import java.util.concurrent.*;

public class Activity2 {
  public static void main(String[] args) {
    int nPermits = 4;
    Semaphore s = new Semaphore(nPermits);
    Philosopher p0 = new Philosopher(0, s);
    Philosopher p1 = new Philosopher(1, s);
    Philosopher p2 = new Philosopher(2, s);
    Philosopher p3 = new Philosopher(3, s);
    Philosopher p4 = new Philosopher(4, s);

    p0.start();
    p1.start();
    p2.start();
    p3.start();
    p4.start();
    
    try {
      p0.join();
      p1.join();
      p2.join();
      p3.join();
      p4.join();
    } catch (InterruptedException e) {
      System.out.println(e);
    }
  }
}