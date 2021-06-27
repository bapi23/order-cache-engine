#pragma once

#include <cassert>
#include <iostream>

#include <vector>
#include <queue>
#include <memory>
#include <optional>
#include "Order.hpp"


class Matcher{

public:
    Matcher(const Order& sellOrder):
        sellOrder(sellOrder)
    {
    }

    std::optional<Order> addBuyOrder(Order order){
        assert(sellOrder.companyName != order.companyName);

        int maxMatched = std::min(order.quantity, sellOrder.quantity);
        sellOrder.quantity -= maxMatched;
        auto remainingQuantity = order.quantity - maxMatched;

        std::optional<Order> remainingOrder;

        if(remainingQuantity > 0){
            remainingOrder = order;
            remainingOrder->quantity = remainingQuantity;
        }

        order.quantity = maxMatched;
        orders.emplace(order.orderID, order);

        return remainingOrder;
    }

    void removeBuyOrder(const std::string& orderID){
        auto foundIt = orders.find(orderID);
        if(foundIt != orders.end()){
            const auto quantity_to_add = foundIt->second.quantity;
            sellOrder.quantity += quantity_to_add;
            orders.erase(orderID);
        }
    }

    int getMatchedQuantity() const {
        return sellOrder.initialQuantity - sellOrder.quantity;
    }

    bool hasRemainingSellQuantity(){
        return sellOrder.quantity > 0;
    }

    std::string sellerCompanyName(){
        return sellOrder.companyName;
    }

    std::vector<Order> getAllOrders(){
        std::vector<Order> output;
        for(const auto& order: orders){
            output.push_back(order.second);
            std::cout << "Removing order " << order.second.quantity << std::endl;
        }
        return output;
    }

    Order sellOrder;
    //Unordered map because order doesn't matter inside matcher.
    std::unordered_map<std::string, Order> orders;
    
};