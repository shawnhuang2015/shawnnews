import {
  Component,
  OnInit,
  Input,
  Inject,
  Injectable,
  trigger,
  state,
  style,
  transition,
  animate
} from '@angular/core';

@Injectable()
export class Config {
  value = 'test injectable';

  getValue() {
    return this.value;
  }
};

@Component({
  selector: 'app-foot',
  templateUrl: './foot.component.html',
  styleUrls: ['./foot.component.scss'],
  host: {
    'class': 'testhost',
    // '(mouseover)': 'onKeyUp($event)',
    '(click)': 'onKeyUp("click")'
  },
  providers: [Config],
  animations: [
    trigger('State', [
      state('a', style({
        backgroundColor: '#00e',
        transform: 'scale(0.5)'
      })),
      state('b', style({
        backgroundColor: '#0f080c',
        transform: 'scale(1.0)'
      })),
      state('c', style({
        backgroundColor: '#f00',
        transform: 'scale(1.5)'
      })),
      transition('b => a', animate('200ms ease-in')),
      transition('a => b', animate('200ms ease-out')),
      transition('b => c', animate('200ms ease-out')),
      transition('c => *', animate('1000ms ease-in'))
    ])
  ]
})

export class FootComponent implements OnInit {
  @Input() value2: string;

  doClass: boolean;

  state: number;
  stateValue: string;

  constructor(private Config: Config) {
    // this.value2 = 'default';
    this.doClass = true;
    this.state = 0;
    this.stateValue = 'a';

    console.log(this.Config);
  }

  ngOnInit() {
  }

  onKeyUp(ev: any) {
    this.state++;

    switch (this.state % 5) {
      case 0:
        this.stateValue = 'a';
      break;
      case 1:
        this.stateValue = 'b';
      break;
      case 2:
        this.stateValue = 'c';
      break;
      case 3:
        this.stateValue = 'b';
      break;
      default:
        this.stateValue = 'c';
      break;
    }
  }
}
