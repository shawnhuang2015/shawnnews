import { Component, OnInit, Input } from '@angular/core';

@Component({
  selector: 'app-foot',
  templateUrl: './foot.component.html',
  styleUrls: ['./foot.component.scss'],
  host: {
    'class': 'testhost'
  }
})

export class FootComponent implements OnInit {
  @Input() value2: string;

  constructor() {
    // this.value2 = 'default';
  }

  ngOnInit() {

  }
}
