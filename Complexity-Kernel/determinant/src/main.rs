use std::ops::Add;
use std::ops::Mul;
use std::ops::Sub;
use std::ops::Div;

const MATRIX_LEN: usize = 32;

fn gcd(mut l: i128, mut r: i128) -> i128 {
  if l < r {
    let tmp: i128 = l;
    l = r;
    r = tmp;
  }
  while r != 0 {
    let tmp: i128 = r;
    r = l % r;
    l = tmp;
  }
  return l;
}


fn i128abs(num: i128) -> i128 {
    if num < 0 {
        return -num;
    } else {
        return num;
    }
}


struct Fraction {
    numerator: i128,
    denominator: i128,
}


impl Fraction {
    fn abs(&self) -> Fraction {
        let numerator = i128abs(self.numerator);
        let denominator = i128abs(self.denominator);
        return Fraction { numerator: numerator, denominator: denominator };
    }

    fn to_i128(&self) -> i128 {
        return self.numerator / self.denominator;
    }

    fn norm(&self) -> Fraction {
        if self.numerator == 0 {
            return Fraction {numerator: 0, denominator: 1};
        }
        let m_gcd = gcd(i128abs(self.numerator), i128abs(self.denominator));
        return Fraction { numerator: self.numerator / m_gcd, denominator: self.denominator / m_gcd };
    }

    fn is_zero(&self) -> bool {
        if self.numerator == 0 {
            return true;
        } else {
            return false;
        }
    }
}


impl Add for Fraction {
    type Output = Fraction;

    fn add(self, other: Fraction) -> Fraction {
        let m_gcd = gcd(i128abs(self.denominator), i128abs(other.denominator));
        let m_lcm = self.denominator * (other.denominator / m_gcd);
        let self_numerator = self.numerator * (m_lcm / self.denominator);
        let other_numerator = other.numerator * (m_lcm / other.denominator);
        let fraction = Fraction { numerator: self_numerator + other_numerator, denominator: m_lcm };
        return fraction.norm();
    }
}


impl Sub for Fraction {
    type Output = Fraction;

    fn sub(self, other: Fraction) -> Fraction {
        let m_gcd = gcd(i128abs(self.denominator), i128abs(other.denominator));
        let m_lcm = self.denominator * (other.denominator / m_gcd);
        let self_numerator = self.numerator * (m_lcm / self.denominator);
        let other_numerator = other.numerator * (m_lcm / other.denominator);
        let fraction = Fraction { numerator: self_numerator - other_numerator, denominator: m_lcm };
        return fraction.norm();
    }
}


impl Mul for Fraction {
    type Output = Fraction;

    fn mul(self, other: Fraction) -> Fraction {
        let fraction = Fraction { numerator: self.numerator * other.numerator, denominator: self.denominator * other.denominator };
        return fraction.norm();
    }
}


impl Div for Fraction {
    type Output = Fraction;

    fn div(self, other: Fraction) -> Fraction {
        let fraction = Fraction { numerator: other.denominator, denominator: other.numerator};
        let fraction = Fraction { numerator: self.numerator * fraction.numerator, denominator: self.denominator * fraction.denominator};
        return fraction.norm();
    }
}


impl Copy for Fraction { }


impl Clone for Fraction {
    fn clone(&self) -> Fraction {
        *self
    }
}


fn determinant(matrix: &mut [Fraction; MATRIX_LEN * MATRIX_LEN])->i64 {
  let mut det: Fraction = Fraction { numerator: 1, denominator: 1};

  for i in 0..MATRIX_LEN {
    let mut max_loc = i;
    for j in (i + 1)..MATRIX_LEN {
      if matrix[j * MATRIX_LEN + i].is_zero() == false {
        max_loc = j;
        break;
      }
    }

    if max_loc != i {
      det = det * Fraction { numerator: -1, denominator: 1};
      for j in i..MATRIX_LEN {
        let tmp = matrix[max_loc * MATRIX_LEN + j];
        matrix[max_loc * MATRIX_LEN + j] = matrix[i * MATRIX_LEN + j];
        matrix[i * MATRIX_LEN + j] = tmp;
      }
    }

    for j in (i + 1)..MATRIX_LEN {
      let fraction = matrix[j * MATRIX_LEN + i] / matrix[i * MATRIX_LEN + i];
      for k in i..MATRIX_LEN {
        matrix[j * MATRIX_LEN + k] = matrix[j * MATRIX_LEN + k] - fraction * matrix[i * MATRIX_LEN + k];
      }
    }
    det = det * matrix[i * MATRIX_LEN + i];
  }
  let det: i64 = det.to_i128() as i64;
  return det;
}

fn main() {
    let m : [[i32; MATRIX_LEN]; MATRIX_LEN] = [[-1,0,-1,0,1,1,1,0,1,1,-1,-1,0,1,1,1,-1,0,-1,0,0,1,0,-1,-1,-1,0,1,-1,1,1,1],
            [0,0,1,-1,0,-1,-1,0,1,0,0,1,1,0,0,0,-1,0,-1,-1,-1,-1,1,1,0,-1,-1,0,0,-1,-1,1],
            [-1,0,1,-1,0,1,0,-1,0,-1,0,1,-1,1,-1,-1,0,0,1,-1,-1,-1,0,-1,-1,0,0,1,0,0,-1,1],
            [1,-1,-1,-1,0,-1,0,1,1,-1,-1,0,0,1,-1,1,-1,-1,-1,1,1,0,1,0,-1,1,1,0,1,0,0,0],
            [0,0,-1,0,1,0,1,0,1,1,1,1,0,1,1,1,1,1,-1,-1,0,-1,1,1,-1,0,1,0,-1,0,-1,1],
            [0,0,0,1,1,-1,1,-1,1,0,1,1,0,-1,1,-1,1,-1,0,0,-1,-1,0,0,1,1,-1,-1,1,1,0,1],
            [0,0,0,-1,1,0,0,-1,-1,-1,1,0,-1,-1,-1,-1,0,0,-1,-1,0,1,-1,0,-1,-1,1,0,1,0,1,0],
            [-1,-1,0,-1,-1,1,1,1,-1,0,1,-1,0,0,-1,0,1,0,0,0,0,-1,-1,1,0,-1,-1,1,0,1,-1,-1],
            [0,1,0,0,0,0,1,1,1,-1,-1,1,1,-1,0,0,1,-1,1,-1,-1,1,0,-1,-1,1,1,0,1,-1,1,1],
            [1,0,0,-1,1,-1,1,1,0,-1,-1,-1,1,0,0,1,1,1,0,0,0,-1,-1,1,1,0,0,0,1,1,0,1],
            [-1,1,-1,0,-1,1,-1,1,1,1,0,1,0,1,-1,-1,1,0,1,0,0,1,1,-1,1,-1,-1,0,-1,1,-1,0],
            [0,1,0,-1,1,1,-1,1,1,-1,1,0,-1,-1,-1,-1,1,1,1,1,0,1,-1,0,-1,1,1,0,0,1,1,0],
            [0,-1,1,1,1,1,1,1,-1,-1,-1,0,-1,-1,-1,1,1,0,0,-1,-1,-1,0,-1,0,-1,0,0,-1,0,1,1],
            [1,-1,0,0,1,1,0,0,-1,0,1,-1,0,1,-1,0,1,0,-1,1,-1,0,-1,-1,0,-1,-1,0,1,1,1,-1],
            [-1,-1,-1,1,1,0,-1,-1,0,-1,-1,-1,-1,0,-1,-1,-1,-1,0,-1,1,1,0,1,1,0,-1,0,0,0,0,0],
            [1,0,0,1,0,1,-1,1,0,1,1,-1,0,1,1,1,-1,-1,-1,1,-1,1,-1,0,0,0,-1,0,1,1,0,0],
            [-1,0,-1,1,1,0,-1,-1,0,1,1,1,1,-1,1,-1,0,-1,1,0,-1,-1,-1,0,0,1,1,1,0,0,-1,1],
            [-1,-1,0,0,-1,-1,-1,-1,0,1,1,1,0,-1,1,0,-1,-1,-1,1,-1,-1,1,1,1,1,0,0,0,1,0,-1],
            [-1,0,0,1,0,1,-1,0,0,1,1,1,1,-1,1,0,-1,0,0,1,-1,-1,1,-1,0,-1,-1,0,0,1,1,1],
            [0,-1,1,1,-1,1,1,-1,-1,0,-1,1,0,0,1,-1,0,0,0,-1,0,-1,-1,1,0,-1,1,0,0,1,1,0],
            [0,1,-1,1,-1,1,1,0,0,1,1,0,1,0,0,1,1,1,0,1,-1,0,0,1,-1,0,0,-1,-1,-1,1,0],
            [-1,1,-1,1,1,1,1,1,1,0,0,0,-1,0,-1,1,1,-1,1,0,-1,-1,-1,0,-1,0,-1,0,-1,-1,1,-1],
            [0,1,-1,1,-1,1,-1,-1,0,1,0,-1,-1,-1,-1,-1,0,0,0,0,0,0,-1,0,0,0,-1,-1,0,0,1,1],
            [-1,0,1,-1,0,-1,0,1,1,0,0,1,1,1,-1,1,1,0,1,-1,1,0,-1,-1,1,0,0,-1,0,0,1,1],
            [1,1,-1,-1,-1,1,-1,1,0,-1,0,0,0,1,0,-1,-1,1,1,0,1,1,0,1,-1,0,0,-1,-1,1,-1,0],
            [0,0,1,-1,1,1,0,-1,-1,0,1,1,1,0,0,0,1,0,1,0,1,0,-1,0,0,-1,1,1,-1,-1,-1,1],
            [1,-1,-1,1,0,-1,1,-1,-1,-1,-1,1,-1,0,0,-1,1,-1,1,1,1,-1,-1,0,0,-1,0,-1,-1,0,1,1],
            [1,1,1,1,1,-1,0,0,0,-1,0,0,1,0,0,1,1,1,-1,0,1,1,0,1,-1,1,0,-1,-1,-1,0,1],
            [-1,0,0,0,-1,0,0,0,0,-1,0,0,1,1,1,1,0,1,1,-1,1,0,-1,-1,0,0,0,0,0,0,1,0],
            [1,-1,1,0,-1,0,1,0,-1,-1,0,1,1,0,0,-1,0,1,-1,0,0,0,-1,1,1,1,-1,0,0,0,1,-1],
            [-1,-1,0,1,-1,-1,0,1,1,1,1,-1,-1,0,1,0,-1,-1,1,0,0,-1,1,1,-1,-1,1,1,0,-1,1,1],
            [1,-1,1,0,1,0,0,0,0,1,-1,-1,0,1,0,0,-1,1,-1,0,-1,-1,-1,1,-1,1,1,1,1,0,-1,0]];

    let mut fractions : [Fraction; MATRIX_LEN * MATRIX_LEN] = [Fraction { numerator: 0, denominator: 0}; MATRIX_LEN * MATRIX_LEN];
    for i in 0..MATRIX_LEN {
        for j in 0..MATRIX_LEN {
            let numerator: i128 = m[i][j] as i128;
            let denominator: i128 = 1;
            fractions[i * MATRIX_LEN + j] = Fraction { numerator: numerator, denominator: denominator};
        }
    }
    let det = determinant(&mut fractions);
    println!("{:?}", det);
}
