webpackJsonp([0,3],{

/***/ 152:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_1__angular_http__ = __webpack_require__(307);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_2_rxjs_add_operator_map__ = __webpack_require__(696);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_2_rxjs_add_operator_map___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_2_rxjs_add_operator_map__);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return NewsApiService; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};



var NewsApiService = (function () {
    function NewsApiService(http) {
        this.http = http;
        // this.baseUrl = 'https://hacker-news.firebaseio.co/v0';
        this.baseUrl = 'https://node-hnapi.herokuapp.com';
    }
    NewsApiService.prototype.fetchItems = function (storyType, page) {
        return this.http.get(this.baseUrl + "/" + storyType + "?page=" + page)
            .map(function (response) { return response.json(); });
    };
    NewsApiService.prototype.fetchItem = function (id) {
        return this.http.get(this.baseUrl + "/item/" + id + ".json")
            .map(function (response) { return response.json(); });
    };
    NewsApiService.prototype.fetchComments = function (id) {
        return this.http.get(this.baseUrl + "/item/" + id)
            .map(function (response) { return response.json(); });
    };
    NewsApiService.prototype.fetchUser = function (id) {
        return this.http.get(this.baseUrl + "/user/" + id)
            .map(function (response) { return response.json(); });
    };
    NewsApiService = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Injectable"])(), 
        __metadata('design:paramtypes', [(typeof (_a = typeof __WEBPACK_IMPORTED_MODULE_1__angular_http__["a" /* Http */] !== 'undefined' && __WEBPACK_IMPORTED_MODULE_1__angular_http__["a" /* Http */]) === 'function' && _a) || Object])
    ], NewsApiService);
    return NewsApiService;
    var _a;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/news-api.service.js.map

/***/ },

/***/ 330:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return AppComponent; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};

var AppComponent = (function () {
    function AppComponent() {
        this.title = 'Hello World app works!';
    }
    AppComponent = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Component"])({
            selector: 'app-root',
            template: __webpack_require__(684),
            styles: [__webpack_require__(675)]
        }), 
        __metadata('design:paramtypes', [])
    ], AppComponent);
    return AppComponent;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/app.component.js.map

/***/ },

/***/ 331:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_1__angular_router__ = __webpack_require__(105);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_2__news_api_service__ = __webpack_require__(152);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return ItemCommentsComponent; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};



var ItemCommentsComponent = (function () {
    function ItemCommentsComponent(newsService, route) {
        this.newsService = newsService;
        this.route = route;
    }
    ItemCommentsComponent.prototype.ngOnInit = function () {
        var _this = this;
        this.sub = this.route.params.subscribe(function (params) {
            console.log('item comment: ', JSON.stringify(params));
            var itemID = +params['id'];
            _this.newsService.fetchComments(itemID).subscribe(function (data) {
                _this.item = data;
            }, function (error) { return console.log('Could not load item ' + itemID); }, function () { return console.log('fetch comment completed'); });
        });
    };
    ItemCommentsComponent = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Component"])({
            selector: 'app-item-comments',
            template: __webpack_require__(689),
            styles: [__webpack_require__(680)]
        }), 
        __metadata('design:paramtypes', [(typeof (_a = typeof __WEBPACK_IMPORTED_MODULE_2__news_api_service__["a" /* NewsApiService */] !== 'undefined' && __WEBPACK_IMPORTED_MODULE_2__news_api_service__["a" /* NewsApiService */]) === 'function' && _a) || Object, (typeof (_b = typeof __WEBPACK_IMPORTED_MODULE_1__angular_router__["a" /* ActivatedRoute */] !== 'undefined' && __WEBPACK_IMPORTED_MODULE_1__angular_router__["a" /* ActivatedRoute */]) === 'function' && _b) || Object])
    ], ItemCommentsComponent);
    return ItemCommentsComponent;
    var _a, _b;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/item-comments.component.js.map

/***/ },

/***/ 332:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_1__angular_router__ = __webpack_require__(105);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_2__news_api_service__ = __webpack_require__(152);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return StoriesComponent; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};



var StoriesComponent = (function () {
    function StoriesComponent(newsService, route) {
        this.newsService = newsService;
        this.route = route;
    }
    StoriesComponent.prototype.ngOnInit = function () {
        var _this = this;
        this.typeSub = this.route.data.subscribe(function (data) {
            // let start = new Date().getTime();
            // while (new Date().getTime() < start + 2000) {
            // };
            console.log(JSON.stringify(data));
            _this.storiesType = data.storiesType;
        });
        this.pageSub = this.route.params.subscribe(function (params) {
            console.log(JSON.stringify(params));
            _this.pageNum = +params['page'] ? +params['page'] : 1;
            _this.newsService.fetchItems(_this.storiesType, _this.pageNum)
                .subscribe(function (items) { return _this.items = items; }, function (error) { return console.log('Error Fetching ' + _this.storiesType + ' stories' + error); }, function () {
                _this.listStart = ((_this.pageNum - 1) * 30) + 1;
                window.scrollTo(0, 0);
            });
        });
        // this.newsService.fetchItems('news', 1).subscribe(
        //   items => {
        //     this.items = items;
        //     return this.items;
        //   },
        //   error => console.log(`Error Fetching Stories: ${JSON.stringify(error)}`)
        // );
    };
    StoriesComponent = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Component"])({
            selector: 'app-stories',
            template: __webpack_require__(691),
            styles: [__webpack_require__(682)]
        }), 
        __metadata('design:paramtypes', [(typeof (_a = typeof __WEBPACK_IMPORTED_MODULE_2__news_api_service__["a" /* NewsApiService */] !== 'undefined' && __WEBPACK_IMPORTED_MODULE_2__news_api_service__["a" /* NewsApiService */]) === 'function' && _a) || Object, (typeof (_b = typeof __WEBPACK_IMPORTED_MODULE_1__angular_router__["a" /* ActivatedRoute */] !== 'undefined' && __WEBPACK_IMPORTED_MODULE_1__angular_router__["a" /* ActivatedRoute */]) === 'function' && _b) || Object])
    ], StoriesComponent);
    return StoriesComponent;
    var _a, _b;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/stories.component.js.map

/***/ },

/***/ 333:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_1__angular_router__ = __webpack_require__(105);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_2__angular_common__ = __webpack_require__(70);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_3__news_api_service__ = __webpack_require__(152);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return UserComponent; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};




var UserComponent = (function () {
    function UserComponent(service, route, location) {
        this.service = service;
        this.route = route;
        this.location = location;
    }
    UserComponent.prototype.ngOnInit = function () {
        var _this = this;
        this.sub = this.route.params.subscribe(function (params) {
            var userID = params['id'];
            _this.service.fetchUser(userID).subscribe(function (data) {
                _this.user = data;
            }, function (error) { return console.log('Could not load user'); });
        });
    };
    UserComponent.prototype.goBack = function () {
        this.location.back();
    };
    UserComponent = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Component"])({
            selector: 'app-user',
            template: __webpack_require__(692),
            styles: [__webpack_require__(683)]
        }), 
        __metadata('design:paramtypes', [(typeof (_a = typeof __WEBPACK_IMPORTED_MODULE_3__news_api_service__["a" /* NewsApiService */] !== 'undefined' && __WEBPACK_IMPORTED_MODULE_3__news_api_service__["a" /* NewsApiService */]) === 'function' && _a) || Object, (typeof (_b = typeof __WEBPACK_IMPORTED_MODULE_1__angular_router__["a" /* ActivatedRoute */] !== 'undefined' && __WEBPACK_IMPORTED_MODULE_1__angular_router__["a" /* ActivatedRoute */]) === 'function' && _b) || Object, (typeof (_c = typeof __WEBPACK_IMPORTED_MODULE_2__angular_common__["d" /* Location */] !== 'undefined' && __WEBPACK_IMPORTED_MODULE_2__angular_common__["d" /* Location */]) === 'function' && _c) || Object])
    ], UserComponent);
    return UserComponent;
    var _a, _b, _c;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/user.component.js.map

/***/ },

/***/ 392:
/***/ function(module, exports) {

function webpackEmptyContext(req) {
	throw new Error("Cannot find module '" + req + "'.");
}
webpackEmptyContext.keys = function() { return []; };
webpackEmptyContext.resolve = webpackEmptyContext;
module.exports = webpackEmptyContext;
webpackEmptyContext.id = 392;


/***/ },

/***/ 393:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__polyfills_ts__ = __webpack_require__(520);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__polyfills_ts___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_0__polyfills_ts__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_1__angular_platform_browser_dynamic__ = __webpack_require__(481);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_2__angular_core__ = __webpack_require__(0);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_3__environments_environment__ = __webpack_require__(519);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_4__app___ = __webpack_require__(515);





if (__WEBPACK_IMPORTED_MODULE_3__environments_environment__["a" /* environment */].production) {
    __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_2__angular_core__["enableProdMode"])();
}
__webpack_require__.i(__WEBPACK_IMPORTED_MODULE_1__angular_platform_browser_dynamic__["a" /* platformBrowserDynamic */])().bootstrapModule(__WEBPACK_IMPORTED_MODULE_4__app___["a" /* AppModule */]);
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/main.js.map

/***/ },

/***/ 510:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_platform_browser__ = __webpack_require__(149);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_1__angular_core__ = __webpack_require__(0);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_2__angular_forms__ = __webpack_require__(472);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_3__angular_http__ = __webpack_require__(307);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_4_angular2_moment__ = __webpack_require__(521);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_4_angular2_moment___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_4_angular2_moment__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_5__app_component__ = __webpack_require__(330);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_6__header_header_component__ = __webpack_require__(514);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_7__stories_stories_component__ = __webpack_require__(332);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_8__foot_foot_component__ = __webpack_require__(513);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_9__stories_item_item_component__ = __webpack_require__(518);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_10__news_api_service__ = __webpack_require__(152);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_11__domain_pipe__ = __webpack_require__(512);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_12__stories_item_item_comments_item_comments_component__ = __webpack_require__(331);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_13__angular_common__ = __webpack_require__(70);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_14__angular_router__ = __webpack_require__(105);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_15__app_routes__ = __webpack_require__(511);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_16__stories_item_item_comments_comment_comment_component__ = __webpack_require__(517);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_17__stories_item_item_comments_comment_tree_comment_tree_component__ = __webpack_require__(516);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_18__user_user_component__ = __webpack_require__(333);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return AppModule; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};



















var AppModule = (function () {
    function AppModule() {
    }
    AppModule = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_1__angular_core__["NgModule"])({
            declarations: [
                __WEBPACK_IMPORTED_MODULE_5__app_component__["a" /* AppComponent */],
                __WEBPACK_IMPORTED_MODULE_6__header_header_component__["a" /* HeaderComponent */],
                __WEBPACK_IMPORTED_MODULE_7__stories_stories_component__["a" /* StoriesComponent */],
                __WEBPACK_IMPORTED_MODULE_8__foot_foot_component__["a" /* FootComponent */],
                __WEBPACK_IMPORTED_MODULE_9__stories_item_item_component__["a" /* ItemComponent */],
                __WEBPACK_IMPORTED_MODULE_11__domain_pipe__["a" /* DomainPipe */],
                __WEBPACK_IMPORTED_MODULE_12__stories_item_item_comments_item_comments_component__["a" /* ItemCommentsComponent */],
                __WEBPACK_IMPORTED_MODULE_16__stories_item_item_comments_comment_comment_component__["a" /* CommentComponent */],
                __WEBPACK_IMPORTED_MODULE_17__stories_item_item_comments_comment_tree_comment_tree_component__["a" /* CommentTreeComponent */],
                __WEBPACK_IMPORTED_MODULE_18__user_user_component__["a" /* UserComponent */]
            ],
            imports: [
                __WEBPACK_IMPORTED_MODULE_0__angular_platform_browser__["b" /* BrowserModule */],
                __WEBPACK_IMPORTED_MODULE_2__angular_forms__["a" /* FormsModule */],
                __WEBPACK_IMPORTED_MODULE_3__angular_http__["b" /* HttpModule */],
                __WEBPACK_IMPORTED_MODULE_4_angular2_moment__["MomentModule"],
                __WEBPACK_IMPORTED_MODULE_15__app_routes__["a" /* routing */],
                __WEBPACK_IMPORTED_MODULE_14__angular_router__["b" /* RouterModule */]
            ],
            providers: [
                __WEBPACK_IMPORTED_MODULE_10__news_api_service__["a" /* NewsApiService */],
                { provide: __WEBPACK_IMPORTED_MODULE_13__angular_common__["e" /* APP_BASE_HREF */], useValue: '/' }],
            bootstrap: [__WEBPACK_IMPORTED_MODULE_5__app_component__["a" /* AppComponent */]]
        }), 
        __metadata('design:paramtypes', [])
    ], AppModule);
    return AppModule;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/app.module.js.map

/***/ },

/***/ 511:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_router__ = __webpack_require__(105);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_1__stories_stories_component__ = __webpack_require__(332);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_2__stories_item_item_comments_item_comments_component__ = __webpack_require__(331);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_3__user_user_component__ = __webpack_require__(333);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return routing; });




var routes = [
    { path: '', redirectTo: 'news/1', pathMatch: 'full' },
    { path: 'news/:page', component: __WEBPACK_IMPORTED_MODULE_1__stories_stories_component__["a" /* StoriesComponent */], data: {
            storiesType: 'news'
        } },
    { path: 'newest/:page', component: __WEBPACK_IMPORTED_MODULE_1__stories_stories_component__["a" /* StoriesComponent */], data: {
            storiesType: 'newest'
        } },
    { path: 'show/:page', component: __WEBPACK_IMPORTED_MODULE_1__stories_stories_component__["a" /* StoriesComponent */], data: {
            storiesType: 'show'
        } },
    { path: 'ask/:page', component: __WEBPACK_IMPORTED_MODULE_1__stories_stories_component__["a" /* StoriesComponent */], data: {
            storiesType: 'ask'
        } },
    { path: 'jobs/:page', component: __WEBPACK_IMPORTED_MODULE_1__stories_stories_component__["a" /* StoriesComponent */], data: {
            storiesType: 'jobs'
        } },
    { path: 'item/:id', component: __WEBPACK_IMPORTED_MODULE_2__stories_item_item_comments_item_comments_component__["a" /* ItemCommentsComponent */] },
    { path: 'user/:id', component: __WEBPACK_IMPORTED_MODULE_3__user_user_component__["a" /* UserComponent */] }
];
var routing = __WEBPACK_IMPORTED_MODULE_0__angular_router__["b" /* RouterModule */].forRoot(routes, { useHash: true });
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/app.routes.js.map

/***/ },

/***/ 512:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return DomainPipe; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};

var DomainPipe = (function () {
    function DomainPipe() {
    }
    DomainPipe.prototype.transform = function (url, args) {
        if (url) {
            var domain = '(' + url.split('/')[2] + ')';
            return domain ? domain.replace('www.', '') : '';
        }
    };
    DomainPipe = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Pipe"])({
            name: 'domain'
        }), 
        __metadata('design:paramtypes', [])
    ], DomainPipe);
    return DomainPipe;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/domain.pipe.js.map

/***/ },

/***/ 513:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return FootComponent; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};

var FootComponent = (function () {
    function FootComponent() {
    }
    FootComponent.prototype.ngOnInit = function () {
    };
    FootComponent = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Component"])({
            selector: 'app-foot',
            template: __webpack_require__(685),
            styles: [__webpack_require__(676)]
        }), 
        __metadata('design:paramtypes', [])
    ], FootComponent);
    return FootComponent;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/foot.component.js.map

/***/ },

/***/ 514:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return HeaderComponent; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};

var HeaderComponent = (function () {
    function HeaderComponent() {
    }
    HeaderComponent.prototype.ngOnInit = function () {
    };
    HeaderComponent = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Component"])({
            selector: 'app-header',
            template: __webpack_require__(686),
            styles: [__webpack_require__(677)]
        }), 
        __metadata('design:paramtypes', [])
    ], HeaderComponent);
    return HeaderComponent;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/header.component.js.map

/***/ },

/***/ 515:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__app_component__ = __webpack_require__(330);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_1__app_module__ = __webpack_require__(510);
/* unused harmony namespace reexport */
/* harmony namespace reexport (by used) */ __webpack_require__.d(exports, "a", function() { return __WEBPACK_IMPORTED_MODULE_1__app_module__["a"]; });


//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/index.js.map

/***/ },

/***/ 516:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return CommentTreeComponent; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};

var CommentTreeComponent = (function () {
    function CommentTreeComponent() {
    }
    CommentTreeComponent.prototype.ngOnInit = function () {
    };
    __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Input"])(), 
        __metadata('design:type', Object)
    ], CommentTreeComponent.prototype, "commentTree", void 0);
    CommentTreeComponent = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Component"])({
            selector: 'app-comment-tree',
            template: __webpack_require__(687),
            styles: [__webpack_require__(678)]
        }), 
        __metadata('design:paramtypes', [])
    ], CommentTreeComponent);
    return CommentTreeComponent;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/comment-tree.component.js.map

/***/ },

/***/ 517:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return CommentComponent; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};

var CommentComponent = (function () {
    function CommentComponent() {
    }
    CommentComponent.prototype.ngOnInit = function () {
        this.collapse = false;
    };
    __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Input"])(), 
        __metadata('design:type', Object)
    ], CommentComponent.prototype, "comment", void 0);
    CommentComponent = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Component"])({
            selector: 'app-comment',
            template: __webpack_require__(688),
            styles: [__webpack_require__(679)]
        }), 
        __metadata('design:paramtypes', [])
    ], CommentComponent);
    return CommentComponent;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/comment.component.js.map

/***/ },

/***/ 518:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0__angular_core__ = __webpack_require__(0);
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return ItemComponent; });
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};

// import { NewsApiService } from '../../news-api.service';
var ItemComponent = (function () {
    function ItemComponent() {
    }
    // constructor(private newsService: NewsApiService) { }
    ItemComponent.prototype.ngOnInit = function () {
        // this.newsService.fetchItem(this.itemID).subscribe(
        //   data => {
        //     return this.item = data;
        //   },
        //   error => console.log(`Could not load item : ${this.itemID}. Error: ${JSON.stringify(error)}`));
    };
    __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Input"])(), 
        __metadata('design:type', Object)
    ], ItemComponent.prototype, "item", void 0);
    ItemComponent = __decorate([
        __webpack_require__.i(__WEBPACK_IMPORTED_MODULE_0__angular_core__["Component"])({
            selector: 'story-item',
            template: __webpack_require__(690),
            styles: [__webpack_require__(681)]
        }), 
        __metadata('design:paramtypes', [])
    ], ItemComponent);
    return ItemComponent;
}());
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/item.component.js.map

/***/ },

/***/ 519:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony export (binding) */ __webpack_require__.d(exports, "a", function() { return environment; });
// The file contents for the current environment will overwrite these during build.
// The build system defaults to the dev environment which uses `environment.ts`, but if you do
// `ng build --env=prod` then `environment.prod.ts` will be used instead.
// The list of which env maps to which file can be found in `angular-cli.json`.
var environment = {
    production: false
};
//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/environment.js.map

/***/ },

/***/ 520:
/***/ function(module, exports, __webpack_require__) {

"use strict";
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0_core_js_es6_symbol__ = __webpack_require__(536);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_0_core_js_es6_symbol___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_0_core_js_es6_symbol__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_1_core_js_es6_object__ = __webpack_require__(529);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_1_core_js_es6_object___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_1_core_js_es6_object__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_2_core_js_es6_function__ = __webpack_require__(525);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_2_core_js_es6_function___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_2_core_js_es6_function__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_3_core_js_es6_parse_int__ = __webpack_require__(531);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_3_core_js_es6_parse_int___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_3_core_js_es6_parse_int__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_4_core_js_es6_parse_float__ = __webpack_require__(530);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_4_core_js_es6_parse_float___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_4_core_js_es6_parse_float__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_5_core_js_es6_number__ = __webpack_require__(528);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_5_core_js_es6_number___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_5_core_js_es6_number__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_6_core_js_es6_math__ = __webpack_require__(527);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_6_core_js_es6_math___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_6_core_js_es6_math__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_7_core_js_es6_string__ = __webpack_require__(535);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_7_core_js_es6_string___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_7_core_js_es6_string__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_8_core_js_es6_date__ = __webpack_require__(524);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_8_core_js_es6_date___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_8_core_js_es6_date__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_9_core_js_es6_array__ = __webpack_require__(523);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_9_core_js_es6_array___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_9_core_js_es6_array__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_10_core_js_es6_regexp__ = __webpack_require__(533);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_10_core_js_es6_regexp___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_10_core_js_es6_regexp__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_11_core_js_es6_map__ = __webpack_require__(526);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_11_core_js_es6_map___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_11_core_js_es6_map__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_12_core_js_es6_set__ = __webpack_require__(534);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_12_core_js_es6_set___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_12_core_js_es6_set__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_13_core_js_es6_reflect__ = __webpack_require__(532);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_13_core_js_es6_reflect___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_13_core_js_es6_reflect__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_14_core_js_es7_reflect__ = __webpack_require__(537);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_14_core_js_es7_reflect___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_14_core_js_es7_reflect__);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_15_zone_js_dist_zone__ = __webpack_require__(710);
/* harmony import */ var __WEBPACK_IMPORTED_MODULE_15_zone_js_dist_zone___default = __webpack_require__.n(__WEBPACK_IMPORTED_MODULE_15_zone_js_dist_zone__);
















//# sourceMappingURL=/Users/xiaokehuang/xiaoke/shawnnews/apps/src/polyfills.js.map

/***/ },

/***/ 675:
/***/ function(module, exports) {

module.exports = "@import url(https://fonts.googleapis.com/css?family=Open%20Sans);\nbody {\n  margin-bottom: 0; }\n  @media only screen and (max-width: 768px) {\n    body {\n      margin: 0; } }\n\n#wrapper {\n  background-color: #F6F6EF;\n  position: relative;\n  width: 85%;\n  min-height: 80px;\n  margin: 0 auto;\n  font-family: 'Open Sans', sans-serif;\n  font-size: 15px;\n  height: 100%; }\n  @media only screen and (max-width: 768px) {\n    #wrapper {\n      width: 100%;\n      background-color: #fff; } }\n"

/***/ },

/***/ 676:
/***/ function(module, exports) {

module.exports = "#footer {\n  position: relative;\n  padding: 10px;\n  height: 60px;\n  border-top: 2px solid #b92b27;\n  letter-spacing: 0.7px;\n  text-align: center; }\n  #footer a {\n    color: #b92b27;\n    font-weight: bold;\n    text-decoration: none; }\n    #footer a:hover {\n      text-decoration: underline; }\n  @media only screen and (max-width: 768px) {\n    #footer {\n      display: none; } }\n"

/***/ },

/***/ 677:
/***/ function(module, exports) {

module.exports = "#header {\n  background-color: #b92b27;\n  color: #fff;\n  padding: 6px 0;\n  line-height: 18px;\n  vertical-align: middle;\n  position: relative;\n  z-index: 1;\n  width: 100%; }\n  @media only screen and (max-width: 768px) {\n    #header {\n      height: 50px;\n      position: fixed;\n      top: 0; } }\n  #header a {\n    display: inline; }\n\n.home-link {\n  width: 50px;\n  height: 66px; }\n\n.logo {\n  width: 50px;\n  padding: 3px 8px 0; }\n  @media only screen and (max-width: 768px) {\n    .logo {\n      width: 45px;\n      padding: 0 0 0 10px; } }\n\nh1 {\n  font-weight: bold;\n  display: inline-block;\n  vertical-align: middle;\n  margin: 0;\n  font-size: 16px; }\n  h1 a {\n    color: #fff;\n    text-decoration: none; }\n\n.name {\n  margin-right: 30px;\n  margin-bottom: 2px; }\n  @media only screen and (max-width: 768px) {\n    .name {\n      display: none; } }\n\n.header-text {\n  position: absolute;\n  width: inherit;\n  height: 20px;\n  left: 10px;\n  top: 27px; }\n  @media only screen and (max-width: 768px) {\n    .header-text {\n      top: 22px; } }\n\n.left {\n  position: absolute;\n  left: 60px;\n  font-size: 16px; }\n  @media only screen and (max-width: 768px) {\n    .left {\n      width: 100%;\n      left: 0; } }\n\n.header-nav {\n  display: inline-block; }\n  @media only screen and (max-width: 768px) {\n    .header-nav {\n      float: right;\n      margin-right: 20px; } }\n  .header-nav a {\n    color: #fff;\n    text-decoration: none; }\n    .header-nav a:hover {\n      font-weight: bold; }\n\n.info {\n  position: absolute;\n  right: 20px;\n  font-size: 16px; }\n  @media only screen and (max-width: 768px) {\n    .info {\n      display: none; } }\n  .info a {\n    color: #fff;\n    font-weight: bold;\n    text-decoration: none; }\n"

/***/ },

/***/ 678:
/***/ function(module, exports) {

module.exports = ""

/***/ },

/***/ 679:
/***/ function(module, exports) {

module.exports = ":host > > > a {\n  color: #b92b27;\n  font-weight: bold;\n  text-decoration: none; }\n  :host > > > a:hover {\n    text-decoration: underline; }\n\n.meta {\n  font-size: 13px;\n  color: #696969;\n  font-weight: bold;\n  letter-spacing: 0.5px;\n  margin-bottom: 8px; }\n  .meta a {\n    color: #b92b27;\n    text-decoration: none; }\n    .meta a:hover {\n      text-decoration: underline; }\n  .meta .time {\n    padding-left: 5px; }\n\n@media only screen and (max-width: 768px) {\n  .meta {\n    font-size: 14px;\n    margin-bottom: 10px; }\n    .meta .time {\n      padding: 0;\n      float: right; } }\n\n.meta-collapse {\n  margin-bottom: 20px; }\n\n.deleted-meta {\n  font-size: 12px;\n  color: #696969;\n  font-weight: bold;\n  letter-spacing: 0.5px;\n  margin: 30px 0; }\n  .deleted-meta a {\n    color: #b92b27;\n    text-decoration: none; }\n\n.collapse {\n  font-size: 13px;\n  letter-spacing: 2px;\n  cursor: pointer; }\n\n.comment-tree {\n  margin-left: 24px; }\n\n@media only screen and (max-width: 1024px) {\n  .comment-tree {\n    margin-left: 8px; } }\n\n.comment-text {\n  font-size: 14px;\n  margin-top: 0;\n  margin-bottom: 20px;\n  word-wrap: break-word;\n  line-height: 1.5em; }\n\n.subtree {\n  margin-left: 0;\n  padding: 0;\n  list-style-type: none; }\n"

/***/ },

/***/ 680:
/***/ function(module, exports) {

module.exports = ".main-content {\n  position: relative;\n  width: 100%;\n  min-height: 100vh;\n  -webkit-transition: opacity .2s ease;\n  transition: opacity .2s ease;\n  box-sizing: border-box;\n  padding: 8px 0;\n  z-index: 0; }\n\n.item {\n  box-sizing: border-box;\n  padding: 10px 40px 0 40px;\n  z-index: 0; }\n\n@media only screen and (max-width: 1024px) {\n  .item {\n    padding: 10px 20px 0 40px; } }\n\n@media only screen and (max-width: 768px) {\n  .item {\n    box-sizing: border-box;\n    padding: 110px 15px 0 15px; } }\n\n.head-margin {\n  margin-bottom: 15px; }\n\np {\n  margin: 2px 0; }\n\n.subject {\n  word-wrap: break-word;\n  margin-top: 20px; }\n\na {\n  color: #000;\n  cursor: pointer;\n  text-decoration: none; }\n\n@media only screen and (max-width: 768px) {\n  .laptop {\n    display: none; } }\n\n@media only screen and (min-width: 769px) {\n  .mobile {\n    display: none; } }\n\n.title {\n  font-size: 16px; }\n\n.title-block {\n  text-align: center;\n  text-overflow: ellipsis;\n  white-space: nowrap;\n  overflow: hidden;\n  margin: 0 75px; }\n\n@media only screen and (max-width: 768px) {\n  .title {\n    font-size: 15px; }\n  .back-button {\n    position: absolute;\n    top: 52%;\n    width: 0.6rem;\n    height: 0.6rem;\n    background: transparent;\n    border-top: .3rem solid #B92B27;\n    border-right: .3rem solid #B92B27;\n    box-shadow: 0 0 0 lightgray;\n    -webkit-transition: all 200ms ease;\n    transition: all 200ms ease;\n    left: 4%;\n    -webkit-transform: translate3d(0, -50%, 0) rotate(-135deg);\n            transform: translate3d(0, -50%, 0) rotate(-135deg); } }\n\n.subtext {\n  font-size: 12px; }\n\n.domain, .subtext {\n  color: #696969;\n  font-weight: bold;\n  letter-spacing: 0.5px; }\n\n.domain a {\n  color: #b92b27; }\n\n.subtext a {\n  color: #b92b27; }\n  .subtext a:hover {\n    text-decoration: underline; }\n\n.item-details {\n  padding: 10px; }\n\n.item-header {\n  border-bottom: 2px solid #b92b27;\n  padding-bottom: 10px; }\n\n@media only screen and (max-width: 768px) {\n  .item-header {\n    background-color: #fff;\n    padding: 10px 0 10px 0;\n    position: fixed;\n    width: 100%;\n    left: 0;\n    top: 62px; } }\n\n.loader {\n  background: #B92B27;\n  -webkit-animation: load1 1s infinite ease-in-out;\n  animation: load1 1s infinite ease-in-out;\n  width: 1em;\n  height: 4em; }\n  .loader:before, .loader:after {\n    background: #B92B27;\n    -webkit-animation: load1 1s infinite ease-in-out;\n    animation: load1 1s infinite ease-in-out;\n    width: 1em;\n    height: 4em; }\n  .loader:before, .loader:after {\n    position: absolute;\n    top: 0;\n    content: ''; }\n  .loader:before {\n    left: -1.5em;\n    -webkit-animation-delay: -0.32s;\n    animation-delay: -0.32s; }\n\n.loading-section {\n  height: 70px;\n  margin: 40px 0 40px 40px; }\n  @media only screen and (max-width: 768px) {\n    .loading-section {\n      display: block;\n      position: relative;\n      margin: 45vh 0; } }\n\n.loader {\n  color: #B92B27;\n  text-indent: -9999em;\n  margin: 20px 20px;\n  position: relative;\n  font-size: 11px;\n  -webkit-transform: translateZ(0);\n  transform: translateZ(0);\n  -webkit-animation-delay: -0.16s;\n  animation-delay: -0.16s; }\n  .loader:after {\n    left: 1.5em; }\n  @media only screen and (max-width: 768px) {\n    .loader {\n      margin: 20px auto; } }\n\n@-webkit-keyframes load1 {\n  0%,\n  80%,\n  100% {\n    box-shadow: 0 0;\n    height: 2em; }\n  40% {\n    box-shadow: 0 -2em;\n    height: 3em; } }\n\n@keyframes load1 {\n  0%,\n  80%,\n  100% {\n    box-shadow: 0 0;\n    height: 2em; }\n  40% {\n    box-shadow: 0 -2em;\n    height: 3em; } }\n\n@media only screen and (max-width: 768px) {\n  @-webkit-keyframes load1 {\n    0%,\n    80%,\n    100% {\n      box-shadow: 0 0;\n      height: 4em; }\n    40% {\n      box-shadow: 0 -2em;\n      height: 5em; } }\n  @keyframes load1 {\n    0%,\n    80%,\n    100% {\n      box-shadow: 0 0;\n      height: 3em; }\n    40% {\n      box-shadow: 0 -2em;\n      height: 4em; } } }\n"

/***/ },

/***/ 681:
/***/ function(module, exports) {

module.exports = "p {\n  margin: 2px 0; }\n  @media only screen and (max-width: 768px) {\n    p {\n      margin-bottom: 5px;\n      margin-top: 0; } }\n\na {\n  color: #000;\n  cursor: pointer;\n  text-decoration: none; }\n\n.title {\n  font-size: 16px; }\n\n.subtext-laptop {\n  font-size: 12px;\n  color: #696969;\n  font-weight: bold;\n  letter-spacing: 0.5px; }\n  .subtext-laptop a {\n    color: #b92b27; }\n    .subtext-laptop a:hover {\n      text-decoration: underline; }\n\n.subtext-palm {\n  font-size: 13px;\n  color: #696969;\n  font-weight: bold;\n  letter-spacing: 0.5px; }\n  .subtext-palm a {\n    color: #b92b27; }\n    .subtext-palm a:hover {\n      text-decoration: underline; }\n  .subtext-palm .details {\n    margin-top: 5px; }\n    .subtext-palm .details .right {\n      float: right; }\n\n.domain {\n  color: #696969;\n  font-weight: bold;\n  letter-spacing: 0.5px; }\n\n@media only screen and (max-width: 768px) {\n  .item-laptop {\n    display: none; } }\n\n@media only screen and (min-width: 769px) {\n  .item-mobile {\n    display: none; } }\n\n.item-details {\n  padding: 10px; }\n"

/***/ },

/***/ 682:
/***/ function(module, exports) {

module.exports = "a {\n  color: #b92b27;\n  text-decoration: none;\n  font-weight: bold; }\n  a:hover {\n    text-decoration: underline; }\n\nol {\n  padding: 0 40px;\n  margin: 0; }\n  @media only screen and (max-width: 768px) {\n    ol {\n      box-sizing: border-box;\n      list-style: none;\n      padding: 0 10px; } }\n  ol li {\n    position: relative;\n    -webkit-transition: background-color .2s ease;\n    transition: background-color .2s ease; }\n\n@media only screen and (max-width: 768px) {\n  .list-margin {\n    margin-top: 55px; } }\n\n.main-content {\n  position: relative;\n  width: 100%;\n  min-height: 100vh;\n  -webkit-transition: opacity .2s ease;\n  transition: opacity .2s ease;\n  box-sizing: border-box;\n  padding: 8px 0;\n  z-index: 0; }\n\n.post {\n  padding: 10px 0 10px 5px;\n  -webkit-transition: background-color 0.2s ease;\n  transition: background-color 0.2s ease;\n  border-bottom: 1px solid #CECECB; }\n  .post .itemNum {\n    color: #696969;\n    position: absolute;\n    width: 30px;\n    text-align: right;\n    left: 0;\n    top: 4px; }\n\n.item-block {\n  display: block; }\n\n.nav {\n  padding: 10px 40px;\n  margin-top: 10px;\n  font-size: 17px; }\n  @media only screen and (max-width: 768px) {\n    .nav a {\n      color: #B92B27;\n      text-decoration: none; } }\n  @media only screen and (max-width: 768px) {\n    .nav {\n      margin: 20px 0;\n      text-align: center;\n      padding: 10px 80px;\n      height: 20px; } }\n  .nav .prev {\n    padding-right: 20px; }\n    @media only screen and (max-width: 768px) {\n      .nav .prev {\n        float: left;\n        padding-right: 0; } }\n  @media only screen and (max-width: 768px) {\n    .nav .more {\n      float: right; } }\n\n.job-header {\n  font-size: 15px;\n  padding: 0 40px 10px; }\n  @media only screen and (max-width: 768px) {\n    .job-header {\n      padding: 60px 15px 25px 15px;\n      border-bottom: 2px dotted #b92b27; } }\n\n.loader {\n  background: #B92B27;\n  -webkit-animation: load1 1s infinite ease-in-out;\n  animation: load1 1s infinite ease-in-out;\n  width: 1em;\n  height: 4em; }\n  .loader:before, .loader:after {\n    background: #B92B27;\n    -webkit-animation: load1 1s infinite ease-in-out;\n    animation: load1 1s infinite ease-in-out;\n    width: 1em;\n    height: 4em; }\n  .loader:before, .loader:after {\n    position: absolute;\n    top: 0;\n    content: ''; }\n  .loader:before {\n    left: -1.5em;\n    -webkit-animation-delay: -0.32s;\n    animation-delay: -0.32s; }\n\n.loading-section {\n  height: 70px;\n  margin: 40px 0 40px 40px; }\n  @media only screen and (max-width: 768px) {\n    .loading-section {\n      display: block;\n      position: relative;\n      margin: 45vh 0; } }\n\n.loader {\n  color: #B92B27;\n  text-indent: -9999em;\n  margin: 20px 20px;\n  position: relative;\n  font-size: 11px;\n  -webkit-transform: translateZ(0);\n  transform: translateZ(0);\n  -webkit-animation-delay: -0.16s;\n  animation-delay: -0.16s; }\n  .loader:after {\n    left: 1.5em; }\n  @media only screen and (max-width: 768px) {\n    .loader {\n      margin: 20px auto; } }\n\n@-webkit-keyframes load1 {\n  0%,\n  80%,\n  100% {\n    box-shadow: 0 0;\n    height: 2em; }\n  40% {\n    box-shadow: 0 -2em;\n    height: 3em; } }\n\n@keyframes load1 {\n  0%,\n  80%,\n  100% {\n    box-shadow: 0 0;\n    height: 2em; }\n  40% {\n    box-shadow: 0 -2em;\n    height: 3em; } }\n\n@media only screen and (max-width: 768px) {\n  @-webkit-keyframes load1 {\n    0%,\n    80%,\n    100% {\n      box-shadow: 0 0;\n      height: 4em; }\n    40% {\n      box-shadow: 0 -2em;\n      height: 5em; } }\n  @keyframes load1 {\n    0%,\n    80%,\n    100% {\n      box-shadow: 0 0;\n      height: 3em; }\n    40% {\n      box-shadow: 0 -2em;\n      height: 4em; } } }\n"

/***/ },

/***/ 683:
/***/ function(module, exports) {

module.exports = ":host > > > pre {\n  white-space: pre-wrap; }\n\n.profile {\n  padding: 30px; }\n\n@media only screen and (max-width: 768px) {\n  .profile {\n    padding: 110px 15px 0 15px; }\n  .title-block {\n    font-size: 15px;\n    text-align: center;\n    text-overflow: ellipsis;\n    white-space: nowrap;\n    overflow: hidden;\n    margin: 0 75px; }\n  .back-button {\n    position: absolute;\n    top: 52%;\n    width: 0.6rem;\n    height: 0.6rem;\n    background: transparent;\n    border-top: .3rem solid #B92B27;\n    border-right: .3rem solid #B92B27;\n    box-shadow: 0 0 0 lightgray;\n    -webkit-transition: all 200ms ease;\n    transition: all 200ms ease;\n    left: 4%;\n    -webkit-transform: translate3d(0, -50%, 0) rotate(-135deg);\n            transform: translate3d(0, -50%, 0) rotate(-135deg); }\n  .item-header {\n    border-bottom: 2px solid #b92b27;\n    padding-bottom: 10px;\n    background-color: #fff;\n    padding: 10px 0 10px 0;\n    position: fixed;\n    width: 100%;\n    left: 0;\n    top: 62px;\n    height: 20px; } }\n\n@media only screen and (min-width: 769px) {\n  .mobile {\n    display: none; } }\n\n.main-details .name {\n  color: #b92b27;\n  font-weight: bold;\n  font-size: 32px;\n  letter-spacing: 2px; }\n\n.main-details .age {\n  font-weight: bold;\n  color: #696969;\n  padding-bottom: 0; }\n\n.main-details .right {\n  float: right;\n  color: #b92b27;\n  font-weight: bold;\n  font-size: 32px;\n  letter-spacing: 2px; }\n\n.main-details .back-button {\n  position: absolute;\n  width: 0.6rem;\n  height: 0.6rem;\n  background: transparent;\n  border-top: .3rem solid #B92B27;\n  border-right: .3rem solid #B92B27;\n  box-shadow: 0 0 0 lightgray;\n  -webkit-transition: all 200ms ease;\n  transition: all 200ms ease;\n  left: 4%;\n  -webkit-transform: translate3d(0, -50%, 0) rotate(-135deg);\n          transform: translate3d(0, -50%, 0) rotate(-135deg); }\n\n@media only screen and (max-width: 768px) {\n  .main-details {\n    margin-top: 20px; }\n    .main-details .name {\n      font-size: 18px; } }\n\n@media only screen and (max-width: 768px) {\n  .main-details .right {\n    font-size: 18px; } }\n\n.other-details {\n  word-wrap: break-word; }\n"

/***/ },

/***/ 684:
/***/ function(module, exports) {

module.exports = "<div id='wrapper'>\n  <app-header></app-header>\n  <!-- <app-stories></app-stories> -->\n  <router-outlet></router-outlet>\n  <app-foot></app-foot>\n</div>"

/***/ },

/***/ 685:
/***/ function(module, exports) {

module.exports = "<div id=\"footer\">\n    <p>Show this project some ❤ on\n      <a href=\"\" target=\"_blank\">\n        GraphSQL\n      </a>\n    </p>\n</div>"

/***/ },

/***/ 686:
/***/ function(module, exports) {

module.exports = "<header id=\"header\">\n  <a class=\"home-link\" href=\"/\">\n    <img class=\"logo\" src=\"https://cdn4.iconfinder.com/data/icons/social-messaging-productivity-1/128/play-icon-2-128.png\">\n  </a>\n  <div class=\"header-text\">\n    <div class=\"left\">\n      <h1 class=\"name\">\n        <a [routerLink]=\"['news', 1]\">Angular 2 HN</a>\n      </h1>\n      <span class=\"header-nav\">\n        <a [routerLink]=\"['newest', 1]\">new</a>\n        <!-- <a href=\"/newest/1\">new</a> -->\n        <span class=\"divider\">\n          |\n        </span>\n        <a [routerLink]=\"['show', 1]\">show</a>\n        <span class=\"divider\">\n          |\n        </span>\n        <a [routerLink]=\"['ask', 1]\">ask</a>\n        <span class=\"divider\">\n          |\n        </span>\n        <a [routerLink]=\"['jobs', 1]\">jobs</a>\n      </span>\n    </div>\n    <div class=\"info\">\n      Built with <a href=\"https://cli.angular.io/\" target=\"_blank\">Angular CLI</a>\n    </div>\n  </div>\n</header>"

/***/ },

/***/ 687:
/***/ function(module, exports) {

module.exports = "<ul class=\"comment-list\">\n  <li *ngFor=\"let comment of commentTree\">\n    <app-comment [comment]=\"comment\"></app-comment>\n  </li>\n</ul>"

/***/ },

/***/ 688:
/***/ function(module, exports) {

module.exports = "<div *ngIf=\"!comment.deleted\">\n  <div class=\"meta\" [class.meta-collapse]=\"collapse\">\n    <span class=\"collapse\" (click)=\"collapse = !collapse\">[{{collapse ? '+' : '-'}}]</span>\n    <a [routerLink]=\"['/user', comment.user]\" routerLinkActive=\"active\">{{comment.user}}</a>\n    <span class=\"time\">{{comment.time_ago}}</span>\n  </div>\n  <div class=\"comment-tree\">\n    <div [hidden]=\"collapse\">\n      <p class=\"comment-text\" [innerHTML]=\"comment.content\"></p>\n      <ul class=\"subtree\">\n        <li *ngFor=\"let subComment of comment.comments\">\n          <app-comment [comment]=\"subComment\"></app-comment>\n        </li>\n      </ul>\n    </div>\n  </div>\n</div>\n<div *ngIf=\"comment.deleted\">\n  <div class=\"deleted-meta\">\n    <span class=\"collapse\">[deleted]</span> | Comment Deleted\n  </div>\n</div>"

/***/ },

/***/ 689:
/***/ function(module, exports) {

module.exports = "<div class=\"main-content\">\n  <div class=\"loading-section\" *ngIf=\"!item\">\n    <!-- You can add a loading indicator here if you want to :) -->\n  </div>\n  <div *ngIf=\"item\" class=\"item\">\n    <div class=\"mobile item-header\">\n     <!-- Markup that shows only on mobile (to give the app a\n    responsive mobile feel). Same attributes as below,\n    nothing really new here (but refer to the source\n    file if you're interested) -->\n    </div>\n    <div class=\"laptop\" [class.item-header]=\"item.comments_count > 0 || item.type === 'job'\" [class.head-margin]=\"item.text\">\n      <p>\n        <a class=\"title\" href=\"{{item.url}}\">\n        {{item.title}}\n        </a>\n        <span *ngIf=\"item.domain\" class=\"domain\">({{item.domain}})</span>\n      </p>\n      <div class=\"subtext\">\n        <span>\n        {{item.points}} points by\n          <a [routerLink]=\"['/user', item.user]\">{{item.user}}</a>\n        </span>\n        <span>\n          {{item.time_ago}}\n          <span> |\n            <a [routerLink]=\"['/item', item.id]\">\n              <span *ngIf=\"item.comments_count !== 0\">\n                {{item.comments_count}}\n                <span *ngIf=\"item.comments_count === 1\">comment</span>\n                <span *ngIf=\"item.comments_count > 1\">comments</span>\n              </span>\n              <span *ngIf=\"item.comments_count === 0\">discuss</span>\n            </a>\n          </span>\n        </span>\n      </div>\n    </div>\n<!--     <p class=\"subject\" [innerHTML]=\"item.content\"></p>\n    <p class=\"subject\" [innerHTML]=\"item.comments.length\"></p> -->\n    <app-comment-tree [commentTree]=\"item.comments\"></app-comment-tree>\n  </div>\n</div>"

/***/ },

/***/ 690:
/***/ function(module, exports) {

module.exports = "<!-- item.component.html -->\n\n<div class=\"item-laptop\">\n  <p>\n    <a class=\"title\" href=\"{{item.url}}\">\n      {{item.title}}\n    </a>\n    <span *ngIf=\"item.domain\" class=\"domain\">{{item.url | domain}}</span>\n  </p>\n  <div class=\"subtext-laptop\">\n    <span *ngIf=\"item.user\">\n      {{item.points}} points by\n      <a [routerLink]=\"['/user', item.user]\">{{item.user}}</a>\n    </span>\n    <span>\n      {{item.time_ago}}\n      <span> |\n        <a [routerLink]=\"['/item', item.id]\">\n          <span *ngIf=\"item.comments_count !== 0\">\n            {{item.comments_count}}\n            <span *ngIf=\"item.comments_count === 1\">comment</span>\n            <span *ngIf=\"item.comments_count > 1\">comments</span>\n          </span>\n          <span *ngIf=\"item.comments_count === 0\">discuss</span>\n        </a>\n      </span>\n    </span>\n  </div>\n</div>\n<div class=\"item-mobile\">\n  <!-- Markup that shows only on mobile (to give the app a\n    responsive mobile feel). Same attributes as above\n    nothing really new here (but refer to the source\n    file if you're interested) -->\n  <a class=\"title\" href=\"{{item.url}}\">\n    {{item.title}}\n  </a>\n</div>"

/***/ },

/***/ 691:
/***/ function(module, exports) {

module.exports = "<div class=\"main-content\">\n  <div class=\"loading-section\" *ngIf=\"!items\">\n    <!-- You can add a loading indicator here if you want to :) -->\n  </div>\n  <div *ngIf=\"items\">\n    <ol start=\"{{ listStart }}\">\n      <li *ngFor=\"let item of items\" class=\"post\">\n        <story-item class=\"item-block\" [item]=\"item\"></story-item>\n      </li>\n    </ol>\n    <div class=\"nav\">\n      <a *ngIf=\"listStart !== 1\" [routerLink]=\"['/'+ storiesType, pageNum - 1]\" class=\"prev\">\n        < Prev\n      </a>\n      <a *ngIf=\"items.length === 30\" [routerLink]=\"['/'+ storiesType, pageNum + 1]\" class=\"more\">\n        More >\n      </a>\n    </div>\n  </div>\n</div>"

/***/ },

/***/ 692:
/***/ function(module, exports) {

module.exports = "<div *ngIf=\"user\" class=\"profile\">\n  <div class=\"mobile item-header\">\n    <p class=\"title-block\">\n      <span class=\"back-button\" (click)=\"goBack()\"></span>\n      Profile: {{ user.id }}\n    </p>\n  </div>\n  <div class=\"main-details\">\n    <span class=\"back-button\" (click)=\"goBack()\">  </span>\n    <span class=\"name\">{{ user.id }}</span>\n    <span class=\"right\">{{ user.karma }} ★</span>\n    <p class=\"age\">Created {{ user.created }}</p>\n  </div>\n  <div class=\"other-details\" *ngIf=\"user.about\">\n    <p [innerHTML]=\"user.about\"></p>\n  </div>\n</div>\n"

/***/ },

/***/ 711:
/***/ function(module, exports) {

function webpackEmptyContext(req) {
	throw new Error("Cannot find module '" + req + "'.");
}
webpackEmptyContext.keys = function() { return []; };
webpackEmptyContext.resolve = webpackEmptyContext;
module.exports = webpackEmptyContext;
webpackEmptyContext.id = 711;


/***/ },

/***/ 712:
/***/ function(module, exports, __webpack_require__) {

module.exports = __webpack_require__(393);


/***/ }

},[712]);
//# sourceMappingURL=main.bundle.map