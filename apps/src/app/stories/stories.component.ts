import { Component, OnInit } from '@angular/core';

import { Observable } from 'rxjs/observable';
import { NewsApiService } from '../news-api.service';

@Component({
  selector: 'app-stories',
  templateUrl: './stories.component.html',
  styleUrls: ['./stories.component.scss']
})
export class StoriesComponent implements OnInit {

  items: number[];

  constructor(private newsService: NewsApiService) {
    this.items = Array();
  }

  ngOnInit() {
    this.newsService.fetchItems('news', 1).subscribe(
      items => {
        this.items = items;
        return this.items;
      },
      error => console.log(`Error Fetching Stories: ${JSON.stringify(error)}`)
    );
  }

}
