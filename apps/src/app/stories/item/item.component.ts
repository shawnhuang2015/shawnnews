import { Component, Input, OnInit, NgZone } from '@angular/core';
// import { NewsApiService } from '../../news-api.service';

@Component({
  selector: 'story-item',
  templateUrl: './item.component.html',
  styleUrls: ['./item.component.scss']
})
export class ItemComponent implements OnInit {
  @Input() item;

  constructor(private ngZone: NgZone) { }
  // constructor(private newsService: NewsApiService) { }

  ngOnInit() {
    // this.newsService.fetchItem(this.itemID).subscribe(
    //   data => {
    //     return this.item = data;
    //   },
    //   error => console.log(`Could not load item : ${this.itemID}. Error: ${JSON.stringify(error)}`));
  }
}
