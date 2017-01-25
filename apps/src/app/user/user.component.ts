import { Component, OnInit } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { Location } from '@angular/common';

import { NewsApiService } from '../news-api.service';

@Component({
  selector: 'app-user',
  templateUrl: './user.component.html',
  styleUrls: ['./user.component.scss']
})
export class UserComponent implements OnInit {
  sub: any;
  user;

  constructor(public service: NewsApiService, private route: ActivatedRoute, private location: Location) { }

  ngOnInit() {
    this.sub = this.route.params.subscribe(params => {
      let userID = params['id'];
      this.service.fetchUser(userID).subscribe(data => {
        this.user = data;
      }, error => console.log('Could not load user'));
    });
  }

  goBack() {
    this.location.back();
  }
}
