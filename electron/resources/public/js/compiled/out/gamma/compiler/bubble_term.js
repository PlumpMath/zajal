// Compiled by ClojureScript 1.8.51 {}
goog.provide('gamma.compiler.bubble_term');
goog.require('cljs.core');
goog.require('gamma.ast');
gamma.compiler.bubble_term.parent_bubbles = (function gamma$compiler$bubble_term$parent_bubbles(db,bubble){
return cljs.core.map.call(null,(function (x){
if(cljs.core._EQ_.call(null,new cljs.core.Keyword(null,"if","if",-458814265),new cljs.core.Keyword(null,"head","head",-771383919).cljs$core$IFn$_invoke$arity$1(db.call(null,x)))){
return cljs.core.assoc.call(null,bubble,new cljs.core.Keyword(null,"current","current",-1088038603),x,new cljs.core.Keyword(null,"conditional","conditional",1441892799),true);
} else {
return cljs.core.assoc.call(null,bubble,new cljs.core.Keyword(null,"current","current",-1088038603),x);
}
}),cljs.core.get_in.call(null,db,new cljs.core.PersistentVector(null, 2, 5, cljs.core.PersistentVector.EMPTY_NODE, [new cljs.core.Keyword(null,"current","current",-1088038603).cljs$core$IFn$_invoke$arity$1(bubble),new cljs.core.Keyword(null,"parents","parents",-2027538891)], null)));
});
gamma.compiler.bubble_term.bubble_term = (function gamma$compiler$bubble_term$bubble_term(db,queue){
while(true){
var bubble = cljs.core.first.call(null,queue);
if((bubble == null)){
return db;
} else {
if(cljs.core._EQ_.call(null,new cljs.core.Keyword(null,"root","root",-448657453),new cljs.core.Keyword(null,"current","current",-1088038603).cljs$core$IFn$_invoke$arity$1(bubble))){
var G__8812 = db;
var G__8813 = cljs.core.rest.call(null,queue);
db = G__8812;
queue = G__8813;
continue;
} else {
var info_path = new cljs.core.PersistentVector(null, 3, 5, cljs.core.PersistentVector.EMPTY_NODE, [new cljs.core.Keyword(null,"current","current",-1088038603).cljs$core$IFn$_invoke$arity$1(bubble),new cljs.core.Keyword(null,"shared","shared",-384145993),new cljs.core.Keyword(null,"start","start",-355208981).cljs$core$IFn$_invoke$arity$1(bubble)], null);
var node_info = cljs.core.get_in.call(null,db,info_path);
if(cljs.core.truth_(new cljs.core.Keyword(null,"conditional","conditional",1441892799).cljs$core$IFn$_invoke$arity$1(bubble))){
if((node_info == null)){
var parent_bubbles = gamma.compiler.bubble_term.parent_bubbles.call(null,db,bubble);
var G__8814 = cljs.core.assoc_in.call(null,db,info_path,new cljs.core.PersistentArrayMap(null, 1, [new cljs.core.Keyword(null,"conditional","conditional",1441892799),true], null));
var G__8815 = cljs.core.concat.call(null,cljs.core.rest.call(null,queue),parent_bubbles);
db = G__8814;
queue = G__8815;
continue;
} else {
if(cljs.core.truth_(new cljs.core.Keyword(null,"unconditional","unconditional",1829191128).cljs$core$IFn$_invoke$arity$1(node_info))){
var G__8816 = cljs.core.assoc_in.call(null,db,cljs.core.conj.call(null,info_path,new cljs.core.Keyword(null,"conditional","conditional",1441892799)),true);
var G__8817 = cljs.core.rest.call(null,queue);
db = G__8816;
queue = G__8817;
continue;
} else {
var G__8818 = db;
var G__8819 = cljs.core.rest.call(null,queue);
db = G__8818;
queue = G__8819;
continue;
}
}
} else {
if(((node_info == null)) || ((new cljs.core.Keyword(null,"unconditional","unconditional",1829191128).cljs$core$IFn$_invoke$arity$1(node_info) == null))){
var parent_bubbles = gamma.compiler.bubble_term.parent_bubbles.call(null,db,bubble);
var G__8820 = cljs.core.assoc_in.call(null,db,info_path,cljs.core.assoc.call(null,node_info,new cljs.core.Keyword(null,"unconditional","unconditional",1829191128),(1)));
var G__8821 = cljs.core.concat.call(null,cljs.core.rest.call(null,queue),parent_bubbles);
db = G__8820;
queue = G__8821;
continue;
} else {
var G__8822 = cljs.core.assoc_in.call(null,db,info_path,cljs.core.assoc.call(null,node_info,new cljs.core.Keyword(null,"unconditional","unconditional",1829191128),((1) + new cljs.core.Keyword(null,"unconditional","unconditional",1829191128).cljs$core$IFn$_invoke$arity$1(node_info))));
var G__8823 = cljs.core.rest.call(null,queue);
db = G__8822;
queue = G__8823;
continue;
}
}
}
}
break;
}
});
gamma.compiler.bubble_term.bubble_terms = (function gamma$compiler$bubble_term$bubble_terms(db){
return cljs.core.reduce.call(null,gamma.compiler.bubble_term.bubble_term,db,cljs.core.map.call(null,(function (x){
return cljs.core.map.call(null,(function (y){
return new cljs.core.PersistentArrayMap(null, 2, [new cljs.core.Keyword(null,"start","start",-355208981),new cljs.core.Keyword(null,"id","id",-1388402092).cljs$core$IFn$_invoke$arity$1(x),new cljs.core.Keyword(null,"current","current",-1088038603),y], null);
}),new cljs.core.Keyword(null,"parents","parents",-2027538891).cljs$core$IFn$_invoke$arity$1(x));
}),cljs.core.filter.call(null,(function (p1__8824_SHARP_){
return ((1) < cljs.core.count.call(null,new cljs.core.Keyword(null,"parents","parents",-2027538891).cljs$core$IFn$_invoke$arity$1(p1__8824_SHARP_)));
}),cljs.core.map.call(null,cljs.core.last,db))));
});

//# sourceMappingURL=bubble_term.js.map?rel=1476490292163