#include "Matcher.h"


class SecurityId{

    int getmatchedQuantity(){
        int quantity = 0;
        for(const auto& matcher: matchers){
            quantity+=matcher.getMatchedQuantity();
        }

        return quantity;
    }

    void appendOrder(const Order& order){
        orders.push_back(order);
    }

    void cancelOrder(const std::string userId){

    }

    std::vector<Order> orders;
    std::vector<Matcher> matchers;
}