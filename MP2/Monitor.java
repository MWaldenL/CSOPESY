import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

public class Monitor {
    private Semaphore[] loadingArea;
    private Semaphore[] unloadingArea;
    // private Semaphore mutex, mutex2, mutex3;
    private Semaphore allAboard;
    private Semaphore allAshore;
    private Semaphore boardQueue;
    private Semaphore unboardQueue;

    private int capacity;
    private int nCars;
    // private int nRides;
    private int nBoarders = 0, nUnboarders = 0;
    
    // public AtomicInteger nCarsDone = new AtomicInteger(0);
    private AtomicInteger nPassengers;

    public Monitor(int nCars, int capacity, int nPassengers) {
        // Define our loading areas
        loadingArea = new Semaphore[nCars];
        unloadingArea = new Semaphore[nCars];
        for (int i=0; i < nCars; i++) {
            this.loadingArea[i] = new Semaphore(0);
            this.unloadingArea[i] = new Semaphore(0);
        }
        loadingArea[0].release();
        unloadingArea[0].release();

        this.capacity = capacity;
        allAboard = new Semaphore(0);
        allAshore = new Semaphore(0);
        boardQueue = new Semaphore(0);
        unboardQueue = new Semaphore(0);

        this.nCars = nCars;
        // this.nRides = nRides;
        this.nPassengers = new AtomicInteger(nPassengers);
    }

    private int next(int i) {
        return (i+1) % this.nCars;
    }

    /**
     * [Car] Loads all passengers into the car from the boarding queue. 
     * 
     * @param id the identifier of the car
     */
    public void load(int id) {
        try { // only allow this car to load
            loadingArea[id].acquire();
            System.out.printf("Car[%d] is now loading passengers!\n", id);
            boardQueue.release(this.capacity); // open the boarding queue
            allAboard.acquire(); // wait for all passengers to board
            System.out.printf("All aboard car[%d]!\n", id);
            loadingArea[next(id)].release(); // signal the next car to begin loading
            this.run(id);
        } catch (InterruptedException e) {
        }
    }

    /**
     * [Car] Runs the car for a certain amount of time
     * 
     * @param id the identifier of the car
     */
    public void run(int id) {
        System.out.printf("Car[%d] is running.\n", id);
    }

    /**
     * [Car] Unloads all passengers present in the car
     * 
     * @param id the identifier of the car
     */
    public void unload(int id) {
        try {
            unloadingArea[id].acquire();
            System.out.printf("Car[%d] has finished running, it is now unloading passengers!\n", id);
            unboardQueue.release(this.capacity); // open the boarding queue
            allAshore.acquire();  // wait for everyone to unboard
            System.out.printf("All ashore from car[%d]!\n", id);
            unloadingArea[next(id)].release(); // signal next car to begin unloading
        } catch (InterruptedException e) {
        }
    }

    /** 
     * [Passenger] boards the car
     * 
     * @param id the identifier of the passenger 
     */
    public void board(int id) {
        try {
            boardQueue.acquire(); // wait for boarding queue to open
            System.out.printf("Passenger[%d] is boarding.\n", id);
            synchronized(new Object()) {
                nBoarders++;
                if (nBoarders == this.capacity) {
                    allAboard.release(); // notify the car that all passengers have boarded
                    nBoarders = 0;
                }
            }
        } catch (InterruptedException e) {
        }
    }

    /**
     * [Passenger] Unboards the car
     * 
     * @param id the identifier of the passenger
     */
    public void unboard(int id) {
        try {
            unboardQueue.acquire(); // wait for unboarding queue to open
            System.out.printf("Passenger[%d] is unboarding.\n", id);
            synchronized(new Object()) {
                nUnboarders++;
                if (nUnboarders == this.capacity) {
                    allAshore.release(); // notify the car that all passengers have unboarded
                    nUnboarders = 0;
                }
            }
        } catch (InterruptedException e) {
        } finally {
            nPassengers.getAndDecrement();
        }
    }

    /**
     * Gets the total number of rides
     */
    // public int getRides() {
    //     return nRides;
    // }

    public int getNPassengers() {
        return nPassengers.get();
    }

    public int getCapacity() {
        return capacity;
    }
}