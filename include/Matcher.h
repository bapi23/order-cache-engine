#include <vector>
#include <queue>
#include "Order.hpp"


class Matcher{

    
    void addOrder(const Order& order){
        orders.push_back(order);
    }

    int getQuantity(){
        int quantity = 0;
        for(const auto& order: orders){
            if(order.side == "Buy"){
                quantity -= order.quantity;
            } else {
                quantity += order.quantity;
            }
        }

        return quantity;
    }

    int getMatchedQuantity(){
        int sellQ = 0;
        int buyQ = 0;
        for(const auto& order: orders){
            if(order.side == "Buy"){
                buyQ -= order.quantity;
            } else {
                sellQ += order.quantity;
            }
        }
        return std::min(sellQ, buyQ);
    }

    std::vector<Order> orders;
    
};