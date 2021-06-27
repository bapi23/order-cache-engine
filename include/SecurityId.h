#pragma once

#include <algorithm>
#include <optional>
#include <numeric>
#include <map>

#include "Matcher.h"


class SecurityId{
public:
    int getMatchedQuantity() const {
        return std::accumulate(matchers.begin(), matchers.end(), 0, 
                [](int sum, const Matcher& m){ return sum + m.getMatchedQuantity();});
    }

    void appendOrder(const Order& order){
        if(order.side == "Sell"){
            matchers.emplace_back(order);
            feedMatchersWithQueuedOrders();
        } else {
            buyOrderQueue.push_back(order);
            auto remainingOrder = feedMatchers(order);
            //if no one matcher handled entire buy request:
            if(remainingOrder){
                buyOrderQueue.push_back(*remainingOrder);
            }
        }
        quantityToOrderId.emplace(order.initialQuantity, order.orderID);
        orderIdToQuantity.emplace(order.orderID, order.initialQuantity);
    }

    void cancelOrderByOrderId(const std::string& orderID){

        //Remove buy orders
        for(auto& matcher: matchers){
            matcher.removeBuyOrder(orderID);
        }

        removeSellOrder(orderID);
        updateInternalMapping(orderID);

        feedMatchersWithQueuedOrders();
        
    }

    void cancelOrderAtQuantity(int quantity){
        //Cancel buy orders:
        auto range = quantityToOrderId.equal_range(quantity);
        for (auto it=range.first; it!=range.second; ++it){
            for(auto matcher: matchers){
                matcher.removeBuyOrder(it->second);
                removeSellOrder(it->second);
                orderIdToQuantity.erase(it->second);
            }
        }
        quantityToOrderId.erase(range.first, range.second);
        feedMatchersWithQueuedOrders();
        //Cancel sell orders:
    }

    void cancelOrderAboveQuantity(int quantity){
        //Cancel buy orders:
        auto firstIt = quantityToOrderId.upper_bound(quantity);
        for (auto it=firstIt; it!=quantityToOrderId.end(); ++it){
            for(auto matcher: matchers){
                matcher.removeBuyOrder(it->second);
                removeSellOrder(it->second);
                orderIdToQuantity.erase(it->second);
            }
        }
        quantityToOrderId.erase(firstIt, quantityToOrderId.end());
        feedMatchersWithQueuedOrders();
    }

private:
    void removeSellOrder(const std::string& orderID)
    {
        //Remove sell order
        auto matcherIt = std::find_if(matchers.begin(), matchers.end(), 
        [&orderID](const Matcher& matcher){ return matcher.sellOrder.orderID == orderID; });

        if(matcherIt != matchers.end()){
            std::vector<Order> matcherOrders = matcherIt->getAllOrders();
            buyOrderQueue.insert(buyOrderQueue.end(), matcherOrders.begin(), matcherOrders.end());
            matchers.erase(matcherIt);
        }
    }

    std::optional<Order> feedMatchers(const Order& order){
        std::optional<Order> currentOrder = order;
        for(auto& matcher: matchers){
            if(matcher.hasRemainingSellQuantity() && matcher.sellerCompanyName() != order.companyName){
                currentOrder = matcher.addBuyOrder(*currentOrder);
                if(!currentOrder){
                    return currentOrder;
                }
            }
        }
        return currentOrder;
    }

    void feedMatchersWithQueuedOrders(){
        //check if we can handle now existing buy orders:
        std::vector<Order> resultQueue;
        for(const auto& order: buyOrderQueue){
            auto remainingOrder = feedMatchers(order);
            if(remainingOrder){
                resultQueue.push_back(*remainingOrder);
            }
        }
        buyOrderQueue = resultQueue;
    }

    void updateInternalMapping(const std::string& orderID){
        auto found_quantity = orderIdToQuantity.find(orderID);
        assert(found_quantity != orderIdToQuantity.end());

        auto range = quantityToOrderId.equal_range(found_quantity->second);
        std::vector<std::multimap<int,std::string>::iterator> toErase;
        for (auto it=range.first; it!=range.second; ++it){
            if(it->second == orderID){
                toErase.push_back(it);
            }
        }
        for(const auto& iter: toErase){
            quantityToOrderId.erase(iter);
        }
    }

    std::vector<Matcher> matchers;
    std::vector<Order> buyOrderQueue;
    std::multimap<int, std::string>  quantityToOrderId;
    std::unordered_map<std::string, int>  orderIdToQuantity;
};