#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <random>
#include <queue>

std::mutex lockPrint;  

class Product
{
    private:
        static int productId;
    public:
    Product() = default; //default constructor

    int getId()
    {
        return productId;
    }

    std:: string printName()
    {
        return "product number " + std::to_string(productId++);
    }

};
int Product::productId = 0;   //start with number 0

class BlockingQueue {
   std::mutex mtx;
   std::condition_variable notFull;       
   std::condition_variable notEmpty;   

   std::queue<Product> items;   //queue of products
   int capacity = 150;

   public:
   //function that describe a process of putting product into queue by producer
   //1) if condition is true, then queue has free space to get some new products, so the producer puts a product there and notifies consumer that 'items' is not empty
   // so he can take a product. the process is unlocked for consumer actions.
   //2) if condition is false, then queue has to wait until consumer take the product to free some space in 'items'. The consumer takes a product.
   void put(Product x)
   {
        std::unique_lock<std::mutex> lock(mtx);
        notFull.wait(lock, [this] { return items.size() < capacity; });  
        items.push(x);
        notEmpty.notify_one();
        lock.unlock();
    }
    //1) if 'items' is not empty, then consumer takes a product and start operating it. Also, the product is being deleted from 'items' and producer is notified that
    //'items' is not full (has free space), so he can put another product there.
    //2) if 'items' is empty, then consumer has to wait for producer to put the product in 'items'.
    Product take() {
        std::unique_lock<std::mutex> lock(mtx);
        notEmpty.wait(lock, [this] { return !items.empty(); });
        Product x = items.front();
        items.pop();
        notFull.notify_one();
        lock.unlock();
        return x;
   }

};


BlockingQueue mQueue; //created queue

class Producer
{
    public:
    //function that let producer put products in 'items' while the condition for this is true. if there is an error, then program catches it.
    void run()
    {
        try 
        {
            while (true) 
            {
                mQueue.put(produce());
            }
        } 
        catch (...) 
        {

        }
    }
    Product produce() //producer created new product
    {
        return Product();
    }
};

class Consumer
{
    private:
    std:: string consumerName;
    int productsReceived = 0;
    public:
    Consumer(std::string name)
    {
        consumerName = name;
    }
    //function that lets consumer take product from 'items' to operate while the condition for this is true. if there is an error, then program catches it.
    void run()
    {
        try 
        {
            while (true) 
            {
                consume(mQueue.take());
            }
        } 
        catch (...) 
        {
        }
    }
    //function that print message in console when consumer get a product to operate and when the process is finished
    void consume(Product product) 
    {
        lockPrint.lock();  //is needed to avoid a race of threads
        int number = product.getId();
        std::cout << consumerName << " got " << product.printName() << " to operate" <<"\n";
        lockPrint.unlock();  
        std::this_thread::sleep_for(std::chrono::seconds(5 + std::rand() % 5));  //a break between starting and finishing points of operating process
        lockPrint.lock();  //the same as in 121 line
        std::cout << consumerName << " finished operating with product number " << number << "\n";
        lockPrint.unlock();
    }

};



int main() {
//create producer and consumers 
    Producer prod;
    Consumer cons1("consumer1");
    Consumer cons2("consumer2");

//create threads
    std::thread producerThread(&Producer::run, &prod);
    std::thread consumerThread1(&Consumer::run, &cons1);
    std::thread consumerThread2(&Consumer::run, &cons2);

    producerThread.join();
    consumerThread1.join();
    consumerThread2.join();

    return 0;
}



