/**
 * CSOPESY Mini MP2 - Rollercoaster Problem
 * Names:   Badulis, Keith Gabriel
 *          Elumba, Maria Jose
 *          Lua, Matthew Walden
 *          Shi, Carlos
 *          Zapanta, Richard Alvin
 * Group:   12
 * Section: S15
 */
import java.util.*;
// import java.util.concurrent.*;

public class Main {
    private static Car[] cars;
    private static Passenger[] passengers;

    public static void main(String[] args) {
        int nPassengers, capacity, nCars;
        Scanner scanner = new Scanner(System.in);

        System.out.print("Input the number of passengers: ");
        nPassengers = getIntInput(scanner);
        System.out.print("Input the capacity of the cars: ");
        capacity = getIntInput(scanner);
        System.out.print("Input the number of cars: ");
        nCars = getIntInput(scanner);
        // System.out.print("Input the number of rides: ");
        // nRides = getIntInput(scanner);
        scanner.close();
        System.out.println("\nPassengers: " + nPassengers + "\nCapacity: " + capacity + "\nCars: " + nCars + "\n");
        Monitor monitor = new Monitor(nCars, capacity, nPassengers);
        cars = new Car[nCars];
        passengers = new Passenger[nPassengers];

        if (nPassengers < capacity) {
            System.out.println("Car cannot run due to insufficient passengers.");
            System.exit(0);
        }

        for (int i = 0; i < passengers.length; i++) {
            passengers[i] = new Passenger(i, monitor);
            passengers[i].start();
        }

        for (int i = 0; i < cars.length; i++) {
            cars[i] = new Car(i, monitor);
            cars[i].start();
        }
    }

    public static void interrupt() {
        for (Car c: cars) c.interrupt();
        for (Passenger p: passengers) p.interrupt();
        System.out.println("All rides completed.");
        System.exit(0);
    }

    private static int getIntInput(Scanner s) {
        // while (!s.hasNextInt()) {
        //     System.out.print("Please enter a number: ");
        //     s.next();
        // }
        // return s.nextInt();
        int input = -1;

        do {
            try {
                input = s.nextInt();
                if (input < 0) {
                    System.out.print("Please enter a positive integer: ");
                }
            } catch (NumberFormatException e) {
                System.out.print("Please enter a valid number: ");
            } catch (InputMismatchException e) {
                System.out.print("Please enter a number: ");
                s.next();
            } catch (NoSuchElementException e) {
            }
        } while (input <= 0);

        return input;
    }
}