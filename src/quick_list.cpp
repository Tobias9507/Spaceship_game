#import <vector>;

//enum type
//QuickList QuickList()
//QuickList<Obj, offsetof(Obj, type)>

template <typename T>
struct QuickList2 : public std::vector<T> {
    // Solve how to define grouping
    QuickList2();
    void insert(T t);
    // void remove(int i); ?????????
    // T get(int i); ?????????
};

void insert(T t){

    // Get index of last obj in grouping

    // Function like this
    // insert(t, indItOrWhatever);
}


int main(){

    return 0;
}