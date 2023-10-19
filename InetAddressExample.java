import java.net.*;

public class InetAddressExample {

    public static void main (String[] args){

        try {
            InetAddress address = InetAddress.getLocalHost();
            System.out.println("\n\n\tLocal Host: ");
            System.out.println("\t" + address.getHostName());
            System.out.println("\t" + address.getHostAddress() + "\n\n");
        } catch (UnknownHostException e) {
            System.out.println("Unable to determine this host's address");
        }

        for (int i=0; i < args.length; i++){

            try{
                InetAddress[] addressesList = InetAddress.getAllByName(args[i]);
                System.out.println(args[i] + ":");
                System.out.println("\t" + addressesList[0].getHostName());
                for (int j=0; j < addressesList.length; j++)
                    System.out.println("\t" + addressesList[j].getHostAddress());
            } catch (UnknownHostException e) {

            }
        }

    }
}