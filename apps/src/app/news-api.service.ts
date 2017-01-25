import { Injectable } from '@angular/core';
import { Http } from '@angular/http';
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/operator/map';

@Injectable()
export class NewsApiService {
  baseUrl: string;

  constructor(private http: Http) {
    // this.baseUrl = 'https://hacker-news.firebaseio.co/v0';
    this.baseUrl = 'https://node-hnapi.herokuapp.com';
  }

  fetchItems(storyType: string, page: number): Observable<any> {
    return this.http.get(`${this.baseUrl}/${storyType}?page=${page}`)
      .map(response => response.json());
  }

  fetchComments(id: number): Observable<any> {
    return this.http.get(`${this.baseUrl}/item/${id}`)
      .map(response => response.json());
  }

  fetchUser(id: number): Observable<any> {
    return this.http.get(`${this.baseUrl}/user/${id}`)
      .map(response => response.json());
  }

  fetchSchema(): Observable<any> {
    let url = 'http://192.168.55.101:8079/engine/graph/schema';
    let url2 = 'https://node-hnapi.herokuapp.com';
    return this.http.get(url2)
      .map(response => {
        console.log('Get Schema');
        return response;
      });
  }
}
