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
        } else {
            unmatchedOrdersQueue.push_back(order);
        }
        feedMatchersWithQueuedOrders();

        quantityToOrderId.emplace(order.initialQuantity, order.orderID);
        orderIdToQuantity.emplace(order.orderID, order.initialQuantity);
    }

    void cancelOrderByOrderId(const std::string& orderID){
        removeBuyOrder(orderID);
        removeSellOrder(orderID);
        feedMatchersWithQueuedOrders();

        removeFromInternalQuantityMapping(orderID);
    }

    void cancelOrderAtQuantity(int quantity){
        auto range = quantityToOrderId.equal_range(quantity);
        for (auto it=range.first; it!=range.second; ++it){
            for(auto matcher: matchers){
                removeBuyOrder(it->second);
                removeSellOrder(it->second);
                orderIdToQuantity.erase(it->second);
            }
        }
        quantityToOrderId.erase(range.first, range.second);
        feedMatchersWithQueuedOrders();
    }

    void cancelOrderAboveQuantity(int quantity){
        auto firstIt = quantityToOrderId.upper_bound(quantity);
        for (auto it=firstIt; it!=quantityToOrderId.end(); ++it){
            for(auto& matcher: matchers){
                removeBuyOrder(it->second);
                removeSellOrder(it->second);
                orderIdToQuantity.erase(it->second);
            }
        }
        quantityToOrderId.erase(firstIt, quantityToOrderId.end());
        feedMatchersWithQueuedOrders();
    }

private:

    void removeBuyOrder(const std::string& orderID)
    {
        for(auto& matcher: matchers){
            matcher.removeBuyOrder(orderID);
        }
    }
    void removeSellOrder(const std::string& orderID)
    {
        //Remove sell order
        auto matcherIt = std::find_if(matchers.begin(), matchers.end(), 
        [&orderID](const Matcher& matcher){ return matcher.sellOrder.orderID == orderID; });

        if(matcherIt != matchers.end()){
            std::vector<Order> matcherOrders = matcherIt->collectAllOrders();
            unmatchedOrdersQueue.insert(unmatchedOrdersQueue.end(), matcherOrders.begin(), matcherOrders.end());
            matchers.erase(matcherIt);
        }
    }

    std::optional<Order> feedMatchers(const Order& order){
        std::optional<Order> currentOrder = order;
        for(auto& matcher: matchers){
            if(matcher.hasRemainingSellQuantity() && matcher.sellerCompanyName() != order.companyName){
                orderIdToMatchers[currentOrder->orderID].push_back(&matcher);
                matcherToOrderIds.emplace(&matcher, currentOrder->orderID);
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
        for(const auto& order: unmatchedOrdersQueue){
            auto remainingOrder = feedMatchers(order);
            if(remainingOrder){
                resultQueue.push_back(*remainingOrder);
            }
        }
        unmatchedOrdersQueue = resultQueue;
    }

    void removeFromInternalQuantityMapping(const std::string& orderID){
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

    void removeFromInternalMatchersMapping(){

    }

    std::vector<Matcher> matchers;
    #std::vector<Order> matchedOrders;
    std::vector<Order> unmatchedOrdersQueue;

    std::unordered_map<std::string, int>  orderIdToQuantity;
    std::multimap<int, std::string>  quantityToOrderId;

    std::unordered_map<std::string, std::vector<Matcher*>> orderIdToMatchers;
    std::multimap<Matcher*, std::string> matcherToOrderIds;
};