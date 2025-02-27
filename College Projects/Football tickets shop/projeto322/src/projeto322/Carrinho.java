package projeto322;
import java.util.ArrayList;
import java.util.List;
public class Carrinho {
    List<Ingresso> items;

    public Carrinho(){
      this.items = new ArrayList<Ingresso>();
    }

    public boolean addItens(Ingresso i){
        this.items.add(i);
        if(this.items.contains(i)){
            return true;
        }
        
        return false;
        
    }

    public boolean removeItens(Ingresso i){
        this.items.remove(i);
        if(this.items.contains(i)){
            return false;
        }
        
        return true; 
    }

    public boolean removeAll(){
        this.items.clear();
        return true;
    }

    public List<Ingresso> getItems() {
        return items;
    }
    public void setItems(List<Ingresso> items) {
        this.items = items;
    }
}
