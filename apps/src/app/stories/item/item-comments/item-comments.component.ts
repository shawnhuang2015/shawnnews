import { Component, OnInit } from '@angular/core';
import { ActivatedRoute } from '@angular/router';

import { NewsApiService } from '../../../news-api.service';

@Component({
  selector: 'app-item-comments',
  templateUrl: './item-comments.component.html',
  styleUrls: ['./item-comments.component.scss']
})
export class ItemCommentsComponent implements OnInit {

  sub: any;
  item;

  constructor(private newsService: NewsApiService, private route: ActivatedRoute) { }

  ngOnInit() {
    this.sub = this.route.params.subscribe(params => {
      console.log('item comment: ', JSON.stringify(params));
      let itemID = +params['id'];
      this.newsService.fetchComments(itemID).subscribe(data => {
        this.item = data;
      }, error => console.log('Could not load item ' + itemID),
      () => console.log('fetch comment completed'));
    });
  }

}
