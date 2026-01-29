package Loteria;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Random;

public class MaszynaLosująca {
    private ArrayList<Kolektura> wszystkieKolektury;
    private String[] imiona = {
            "Adam", "Adrian", "Aleksander", "Alan", "Anastazy", "Amadeusz",
            "Arek", "Alojzy", "Artur", "Borys", "Bartłomiej", "Bartek",
            "Bronisław", "Cezary", "Dariusz", "Daniel", "Damian", "Dawid",
            "Franciszek", "Filip", "Florian", "Grzegorz", "Gracjan", "Henryk",
            "Igor", "Ireneusz", "Jan", "Jarosław", "Jerzy", "Jakub", "Karol",
            "Kacper", "Krystian", "Krzysztof", "Konrad", "Kajetan", "Leon",
            "Ludwik", "Marian", "Marcel", "Michał", "Mikołaj", "Maciej", "Mirosław",
            "Mariusz", "Maksymilian", "Maksym", "Nataniel", "Norman", "Patryk",
            "Piotr", "Ryszard", "Radosław", "Rafał", "Roman", "Robert", "Stanisław",
            "Sławomir", "Taduesz", "Wacław", "Zenon", "Bogdan", "Nikodem", "Sebastian",
            "Mieczysław", "Aleksy", "Marek", "Janusz", "Miłosz", "Wiesław"
            };
    private String[] nazwiska = {
            "Nowak", "Kowalski", "Wiśniewski", "Wójcik", "Zieliński", "Lis",
            "Marciniak", "Dudek", "Mazur", "Dąbrowski", "Kaczmarek", "Lewandowski",
            "Piasek", "Kosakowski", "Małysz", "Duda", "Zielonka", "Czarnecki",
            "Rybak", "Łukaszewski", "Malinowski", "Stanowski", "Borowiecki",
            "Czerkas", "Sadowski", "Szpak", "Witkowski", "Brzoza", "Sapkowski",
            "Zając", "Szydło", "Nawrocki", "Kałduński", "Kamrowski", "Dobry",
            "Ptach", "Andrzejuk", "Chodakowski", "Piaseczny", "Sadowski", "Maj",
            "Jankowski", "Kamiński", "Woźniak", "Krawczyk", "Wojciechowski",
            "Król", "Wieczorek", "Jabłoński", "Nowakowski", "Olszewski", "Jaworski",
            "Adamczyk", "Stępień", "Dudek", "Górski", "Nowicki", "Pawlak", "Sikora",
            "Rutkowski", "Walczak", "Michalak", "Ostrowski", "Szewczyk", "Tomaszewski",
            "Zalewski", "Wróblewski", "Pietrzak", "Jasiński", "Borowski", "Włodarczyk",
            "Chmielewski", "Sokołowski", "Sawicki", "Wysocki", "Maciejewski",
            "Kaźmierczak", "Urbański", "Konieczny", "Krupa", "Głowacki", "Kozak",
            "Wasilewski", "Gajewski", "Szulc", "Baranowski", "Szymczak", "Drygas",
            "Jaroń", "Widłak", "Wilczyński", "Dudziński", "Malon", "Czerkas",
            "Pawlik", "Mikołajczyk"
     };
    private Random random = new Random();

    public MaszynaLosująca(){
        wszystkieKolektury = new ArrayList<>();

    }
    public void nowaKolektura(Kolektura kolektura){
        wszystkieKolektury.add(kolektura);
    }

    public Kolektura losowaKolektura(){
        int indeks = random.nextInt(wszystkieKolektury.size());
        return wszystkieKolektury.get(indeks);
    }

    public int losowaLiczbaLosowań(){
        return random.nextInt(10) + 1;
    }

    public int losowaLiczbaZakładów(){
        return random.nextInt(8) + 1;
    }

    public String Imię(){
        int indeks = random.nextInt(imiona.length);
        return imiona[indeks];
    }

    public String Nazwisko(){
        int indeks = random.nextInt(nazwiska.length);
        return nazwiska[indeks];
    }

    public String Pesel(){
        StringBuilder sb = new StringBuilder();
        for(int i = 0; i < 11; ++i)
            sb.append(random.nextInt(10));
        return sb.toString();
    }

    public long Złote(){
        long liczba = random.nextLong(1000000);
        long mod = random.nextLong(1000000);
        return liczba % mod;
    }

    public long Grosze(){
        return random.nextLong(100);
    }

    public int[] zestawLiczb(){
        int[] wynik = new int[6];
        boolean czyByła[] = new boolean[49];

        int ile = 0;
        Random random = new Random();
        while(ile < 6){
            int liczba = random.nextInt(49);
            if(czyByła[liczba] == false) {
                wynik[ile] = liczba + 1;
                ++ile;
            }
            czyByła[liczba] = true;
        }
        Arrays.sort(wynik);
        return wynik;
    }

    public int[] zestawIndeksów(int maks){
        int ileLiczb = random.nextInt(maks) + 1;
        int[] wynik = new int[ileLiczb];
        boolean czyByła[] = new boolean[maks];

        int ile = 0;
        while(ile < ileLiczb){
            int liczba = random.nextInt(maks);

            if(czyByła[liczba] == false) {
                wynik[ile] = liczba;
                ++ile;
            }
            czyByła[liczba] = true;
        }
        Arrays.sort(wynik);
        return wynik;
    }

    public int indeks(int maks){
        return random.nextInt(maks);
    }

    public int losowaLiczbaKuponów(){ return random.nextInt(100) + 1;}

}
