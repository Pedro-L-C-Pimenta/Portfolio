package projeto322;

import java.util.List;
import java.util.*;
import java.sql.Time;
import java.time.LocalTime;

public class Clube {
    private final String nome;
    private List<Partida> partidas;
    private String estádio;
    
    //Construtor
    public Clube(String nome, String estádio) {
        this.nome = nome;
        this.estádio = estádio;
        this.partidas = new ArrayList<Partida>();
    }
    //Getters e Setters
    public String getNome() {
        return nome;
    }

    public void setPartidas(List<Partida> partidas) {
        this.partidas = partidas;
    }
    public List<Partida> getPartidas() {
        return partidas;
    }
    
    public String getEstádio() {
        return estádio;
    }
    public void setEstádio(String estádio) {
        this.estádio = estádio;
    }

    //Métodos de partidas
    public boolean adicionaPartida(Date data, LocalTime horário, Clube adversário, String tipoMandoCampo){
        if(tipoMandoCampo.equals("mandante")){
            Partida partida = new Partida(data, horário, this ,adversário);
            partidas.add(partida);
            if(partidas.contains(partida)){
                return true;
            } else {
                return false;
            }
        } else if(tipoMandoCampo.equals("visitante")){
            Partida partida = new Partida(data, horário, adversário, this);
            partidas.add(partida);
            if(partidas.contains(partida)){
                return true;
            } else {
                return false;
            }
        } else {
            System.out.println("Erro de definição do Mando de Campo, use 'mandante' ou 'visitante'");
            return false;
        }
    }

}
