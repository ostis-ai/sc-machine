export class Error {

  public static Cirtial(msg: string) {
    throw `Critical: ${msg}`;
  }
  
  public static InvalidState() {
    Error.Cirtial('Invalid state');
  }

  public static Assert(exp: boolean, msg: string = 'no description') {
    if (!exp) {
      throw `Assert: ${msg}`;
    }
  }
  
}