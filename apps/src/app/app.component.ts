import { Component, OnInit, NgZone } from '@angular/core';
import { NewsApiService } from './news-api.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  title = 'Hello World app works!';

  constructor(private service: NewsApiService, private ngZone:NgZone) {
    this.ngZone.onStable.subscribe(() => {
      console.log('We are stable');
    });
    this.ngZone.onUnstable.subscribe(() => {
      console.log('We are unstable');
    });
  }

  OnInit() {
    this.title = 'Testing case';
  }
}
