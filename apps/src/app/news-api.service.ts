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

  fetchItem(id: number): Observable<any> {
    return this.http.get(`${this.baseUrl}/item/${id}.json`)
    .map(response => response.json());
  }
}
