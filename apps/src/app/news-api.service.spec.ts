/* tslint:disable:no-unused-variable */

import { TestBed, async, inject } from '@angular/core/testing';
import { NewsApiService } from './news-api.service';
import { AppModule } from './app.module';

describe('NewsApiService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [NewsApiService],
      imports: [AppModule]
    });
  });

  it('should ...', inject([NewsApiService], (service: NewsApiService) => {
    expect(service).toBeTruthy();

    return service.fetchUser((<any>'capocannoniere')).subscribe(result => {
      expect(result).toBeDefined();
    });

  }));

  it('Observable Test ...', inject([NewsApiService], (service: NewsApiService) => {
    return service.fetchSchema().subscribe(result => {
      expect(result).toBeDefined();
    });
  }));

  it('Observable Test ...', inject([NewsApiService], (service: NewsApiService) => {
    service.fetchComments(13484372).subscribe(result => {
      expect(result).toBeDefined();
    });
  }));
});
