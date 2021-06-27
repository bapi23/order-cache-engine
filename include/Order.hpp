#pragma once

#include <cassert>

#include <string>

class Order{
public:
    Order(){}
    Order(const std::string& orderID,
          const std::string& securityID,
          const std::string& side,
          int quantity,
          const std::string& userID,
          const std::string& companyName):
          orderID(orderID),
          securityID(securityID),
          side(side),
          initialQuantity(quantity),
          quantity(quantity),
          remainingQuantity(quantity),
          userID(userID),
          companyName(companyName)
          {} 

    void reserve(int quantityToReserve){
        assert(quantity >= quantityToReserve);
        remainingQuantity -= quantityToReserve;

    }

    std::string orderID;     // e.g  "ID1" (unique id for order)
    std::string securityID;  // e.g. "US9128473801"
    std::string side;        // e.g. “Buy” or “Sell” 
    int initialQuantity;
    int quantity;            // e.g. 120000
    int remainingQuantity;
    std::string userID;      // e.g. “John Smith” 
    std::string companyName; // e.g. “Hedge Fund A” (company of User ID)

};