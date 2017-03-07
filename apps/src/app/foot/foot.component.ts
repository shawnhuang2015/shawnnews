import { Component, OnInit, Input } from '@angular/core';

@Component({
  selector: 'app-foot',
  templateUrl: './foot.component.html',
  styleUrls: ['./foot.component.scss'],
  host: {
    'class': 'testhost',
    '(mouseover)': 'onKeyUp($event)',
    '(click)': 'onKeyUp("click")'
  }
})

export class FootComponent implements OnInit {
  @Input() value2: string;

  doClass: boolean;

  constructor() {
    // this.value2 = 'default';
    this.doClass = true;
  }

  ngOnInit() {
  }

  onKeyUp(ev: any) {
    // do something meaningful with it
    console.log(`The user just pressed ${ev}!`);
  }
}
