public class Car extends Thread {
    int ID;
    Monitor monitor;

    public Car(int ID, Monitor monitor) {
        this.ID = ID;
        this.monitor = monitor;
    }

    @Override
    public void run() {
        while (monitor.getNPassengers() > 0 && monitor.getNPassengers() >= monitor.getCapacity()) {
            monitor.load(ID);
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            monitor.unload(ID);
        }
        if (monitor.getNPassengers() < monitor.getCapacity() && monitor.getNPassengers() != 0)
            System.out.println("Car cannot run due to insufficient passengers.");
        Main.interrupt();
    }
}
