import { Component, OnInit } from '@angular/core';

// import { Observable } from 'rxjs/observable';

import { ActivatedRoute } from '@angular/router';

import { NewsApiService } from '../news-api.service';

@Component({
  selector: 'app-stories',
  templateUrl: './stories.component.html',
  styleUrls: ['./stories.component.scss']
})
export class StoriesComponent implements OnInit {

  items;
  typeSub: any;
  pageSub: any;
  storiesType: any;
  pageNum: any;
  listStart: number;

  constructor(private newsService: NewsApiService, private route: ActivatedRoute) {
  }

  ngOnInit() {
    this.typeSub = this.route.data.subscribe(data => {
      // let start = new Date().getTime();
      // while (new Date().getTime() < start + 2000) {
      // };

      console.log(JSON.stringify(data));
      this.storiesType = (data as any).storiesType;
    });

    this.pageSub = this.route.params.subscribe(params => {
      console.log(JSON.stringify(params));
      this.pageNum = +params['page'] ? +params['page'] : 1;
      this.newsService.fetchItems(this.storiesType, this.pageNum)
        .subscribe(
        items => this.items = items,
        error => console.log('Error Fetching ' + this.storiesType + ' stories' + error),
        () => {
          this.listStart = ((this.pageNum - 1) * 30) + 1;
          window.scrollTo(0, 0);
        });

      // this.newsService.fetchSchema().subscribe(result => {
      //   console.log(result.text());
      // });
    });

    // this.newsService.fetchItems('news', 1).subscribe(
    //   items => {
    //     this.items = items;
    //     return this.items;
    //   },
    //   error => console.log(`Error Fetching Stories: ${JSON.stringify(error)}`)
    // );

    // console.log('fetch schema');
    // this.newsService.fetchSchema().subscribe(data => {
    //   console.log(data);
    // });
  }

}
