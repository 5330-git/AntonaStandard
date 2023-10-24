# Improvement Report



## Category

[TOC]

## A brief introductions

The improvement report is mean to report the possible suggestion to improve AntonaStandard, then make it more **efficiency , safety and maintainable** 

Thus, the following contains will describes the improvement suggestions in detail. The suggestions will be divided into pieces by the **versions**  ,so you can easily find the points.

## Before AntonaStandard-v-9.1.0

- The improvements of AntonaStandard-v-9.1.0 are mostly based on the *Effective C++* 

### `Math/Fraction`

- As *Effective C++* **Item 3: Use const whever possible** , the returned value of Fraction `operator` should be declared as `const` , and it would be better to name the arguments as `lhs` and `rhs` to distinguish its position near the operator :

  ```cpp
  const Fraction operator+(const Fraction& lhs,const Fraction& rhs);
  const Fraction operator-(const Fraction& f1,const Fraction& rhs);
  const Fraction operator*(const Fraction& f1,const Fraction& rhs);
  const Fraction operator/(const Fraction& f1,const Fraction& rhs);
  inline const Fraction  operator-();
  // also for member inverse:
  const Fraction inverse()const;
  ```

  

- Using default arguments to fit more forms of usage.

  ```cpp
  Fraction(int num=0,int den=1):numerator(num),denominator(den){};
  ```

- The C++ runtime System can abort the program during division calculation if the denominator is 0, and it's a risk to throw an exception from a constructor.

- As **Item 24: Declare non-member functions when type conversions should apply to all parameters** , we should avoid the friend function whenever we can. So In our project we should cancel the friend declarations and use `getNumerator()` and `getDenominator()` to access the data member. 

- The interface of `alterValue` is not necessary

- moving constructor and moving assignment operator are not necessary



### `MultiplatformSupport::DllSupport` 

- The dll handle is a kind of resource ,so using *RAII* to manage it is a better choice.
- However , the DllSupport just provide a low level encapsulation, it's better to provide another class to encapsulate it:  `Utilities::DllLoader` . And we should pay attention to that `unloadDll` may throw an exception to break the destructor, so we should catch this exception and log it then **abort** the program. Of course as the **Item 8: Prevent exceptions from leaving destructors** we should provide an interface `void tryUnload()` for clients so they can do that themselves.



#### `MultiplatforSupport::Socket...`

- Put the responsibilitis of managing  `sockaddr_t` memory to smart pointer 

- As **Item 28: Avoid returning "handles" to object internals** , it's risky for the following operations:

  ```cpp
  class SocketAddress{
     void* addr_in = nullptr;			// should be alter to std::shared_ptr<void> addr_in;
  public:
      virtual void* getAddrIn();		// risky! returning "handles" to object internals,alter to virtual std::shared_ptr<void> getAddIn();
      ...
  };
  ```

  ```cpp
  class Socket{
  public:
      using Socketid_t = 
      #ifdef AntonaStandard_PLATFORM_WINDOWS
          SOCKET
      #elif AntonaStandard_PLATFORM_LINUX
          int
      #endif
      ;
  private:
      Socketid_t socketid;
      SocketAddress* address;			// should be alter to shared_ptr<SocketAddress> address;
  public:
      
      inline SocketAddress* getAddress(){
          return this->address;				// risky! should alter to shared_ptr<SocketAddress> getAddress();
      }
      void setAddress(SocketAddress* addr);	// risky too! this interface is used for SocketCommunication::createSocket only (should be private and friend to SocketCommunication)
      inline Socketid_t getSocketId() const{
          return socketid;
      }
      inline void setSocketId(Socketid_t skt_id){	// risky! this interface is used for SocketCommunication::createSocket only, and it is a risk when the client calling it with an invalid skt_id, so it's unwise to explose it to the client (so it should be priavte and friend to SocketCommunication)
          this->socketid = skt_id;
      }
  	Socket();
      Socket(Socket& socket);				// alter to deleted
      Socket(Socket&& socket);			// usable
      ~Socket();
      // it's better to provide swap interfaces
  };
  ```

- For `Socket` we should consider the behaviours of copy constructor and moving constructor. Should the ownership of  `socket_id` should be owned by more than one `Socket` instance?  Of course not. So the **copy constructor and assignment operator should be deleted** .But the moving constructor can be reserved.
- `Socket` should guarantee the socket_id is released when its instance is destructed.

- We should guarantee there is only one instance of `SocketCommunication` , to make our program more efficiency. So the alternative way is using local static object:

  ```cpp
  class SocketCommunication{
  private:
      SocketCommunication();
  public:
      static SocketCommunication& get();				// get the local static instance of SocketCommunication.
      /*
      {
          static SocketCommunication sc;
          return sc;
      }
      */
      // copy constructor of SocketCommunication should be deleted!
      ~SocketCommunication();
      Socket createSocket(SocketProtocol prot,
          SocketType type,unsigned short port,
          const char* addr_str,int defualt=0);        
      Socket createSocket(SocketProtocol prot,
          SocketType type,unsigned short port,
          const std::string addr_str,int defualt=0);   
      void bindSocket(Socket& socket);        
      void listenSocket(Socket& socket);     
      Socket acceptSocket(Socket& socket);    
      void connectSocket(Socket& socket);     
      void closeSocket(Socket& socket);       
  
      void send(Socket& socket,SocketDataBuffer& data);         
      size_t receive(Socket& socket,SocketDataBuffer& data);      
  };
  ```

  

#### `ThreadTools::ThreadsPool`

- Before detaching of joining a thread ,it is a must to call `joinable` to judge if the thread is available

- However, the implement of current thread pool is poor , so I decided to alter the existed Thread Pool project then make it available in AntonaStandard:

  https://github.com/lujiawei922/thread-pool.git  

