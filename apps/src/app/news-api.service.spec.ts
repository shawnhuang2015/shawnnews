/* tslint:disable:no-unused-variable */

import { TestBed, async, inject } from '@angular/core/testing';
import { NewsApiService } from './news-api.service';
import { AppModule } from './app.module';

describe('NewsApiService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [NewsApiService],
      imports: [ AppModule ]
    });
  });

  it('should ...', inject([NewsApiService], (service: NewsApiService) => {
    expect(service).toBeTruthy();
    expect(service.baseUrl).toEqual('https://hacker-news.firebaseio.com/v0');
  }));
});
