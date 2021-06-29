#pragma once

#include <cassert>

#include <algorithm>
#include <vector>
#include <queue>
#include <optional>

#include "Order.h"


class Matcher{

public:
    Matcher(Order sellOrder):
        sellOrder(std::move(sellOrder))
    {
    }

    Matcher(const Matcher&) = delete;
    Matcher& operator=(const Matcher&) = delete;

    Matcher(Matcher&& other):
        sellOrder(std::move(other.sellOrder))
    {
        this->orders = std::move(other.orders);
    }
    Matcher& operator=(Matcher&& other){
        this->sellOrder = std::move(other.sellOrder);
        this->orders = std::move(other.orders);
        return *this;
    }


    std::optional<Order> addBuyOrder(Order* order){
        const int matchedQuantity = std::min(order->quantity, sellOrder.quantity);
        sellOrder.quantity -= matchedQuantity;
        const int remainingQuantity = order->quantity - matchedQuantity;

        std::optional<Order> remainingOrder;

        if(remainingQuantity > 0){
            remainingOrder = order->clone();
            remainingOrder->quantity = remainingQuantity;
        }

        order->quantity = matchedQuantity;
        order->setMatcher(this);
        orders.push_back(order);

        return remainingOrder;
    }

    void removeBuyOrder(const std::string& orderID){
        auto foundIt = std::find_if(orders.begin(), orders.end(), [&orderID](const Order* o){return o->orderID == orderID;});
        
        if(foundIt != orders.end()){
            const auto quantity_to_add = (*foundIt)->quantity;
            sellOrder.quantity += quantity_to_add;
            orders.erase(foundIt);
        }
    }

    int getMatchedQuantity() const {
        return sellOrder.initialQuantity - sellOrder.quantity;
    }

    bool hasRemainingSellQuantity(){
        return sellOrder.quantity > 0;
    }

    std::string sellOrderCompanyName(){
        return sellOrder.companyName;
    }

    std::vector<Order*> retrieveAllOrders(){
        for(auto* order: orders){
            order->setMatcher(nullptr);
        }
        
        return std::move(orders);
    }

    std::string getSellOrderID() const {
        return sellOrder.orderID;
    }

    ~Matcher(){
        for(auto* order: orders){
            order->setMatcher(nullptr);
        }
    }

private:
    Order sellOrder;
    std::vector<Order*> orders;
};