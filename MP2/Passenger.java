class Passenger extends Thread {
    int ID;
    Monitor monitor;

    public Passenger(int ID, Monitor monitor) {
        this.ID = ID;
        this.monitor = monitor;
    }

    @Override
    public void run() {  
        try {
            System.out.printf("Passenger[%d] is wandering.\n", ID);
            Thread.sleep(2000 + (long) (Math.random() * 10000));
        } catch (InterruptedException e) {
        }
        monitor.board(ID);
        monitor.unboard(ID);
    }
}